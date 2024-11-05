#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "fileUtils.h"
#include "commonUtils.h"

std::string artFilePath = "content/art/";

namespace FileUtils
{

  std::vector<std::string> readLinesFromFile(const std::string &filePath)
  {
    /*
      input: filepath
      output: string vector contains lines from the files, each line one element
      use: include "fileUtils.h" first, then call namespace::function()
        e.g., FileUtils::readLinesFromFile(filepath)
    */

    std::vector<std::string> lines; // declare a vector (dynamic array) which element type are string
    std::ifstream file(filePath);   // read file from the filePath parameter
    std::string line;

    if (!file.is_open()) // if file didnt open successfully
    {
      Logger::logMessage("[ERROR] Failed to open file: " + filePath);
      return lines; // Return an empty vector if the file cannot be opened
    }

    while (getline(file, line)) // append the read line into the vector "lines"
    {
      lines.push_back(line);
    }

    file.close();
    return lines;
  }

  std::vector<std::string> applyColorInstructions(const std::vector<std::string> &artLines, const std::vector<std::string> &colorLines)
  {
    std::vector<std::string> coloredArtLines;

    if (artLines.empty() || colorLines.empty())
    {
      Logger::logMessage("[ERROR] [fileUtils/applyColorInstructions] artLines or colorLines is empty");
      return coloredArtLines;
    }
    else if (artLines.size() != colorLines.size())
    {
      Logger::logMessage("[ERROR] [fileUtils/applyColorInstructions] artLines' and colorLines' size is different");
      return coloredArtLines;
    }

    for (size_t i = 0; i < artLines.size(); ++i)
    {
      std::string currentLine = artLines[i];
      std::string coloredLine = "";
      if (i < colorLines.size() && !colorLines[i].empty())
      {
        std::istringstream iss(colorLines[i]);
        int end, prevEnd = 0;
        std::string color;
        while (iss >> end >> color)
        {
          if (AnsiColors::colorMap_strMap.find(color) != AnsiColors::colorMap_strMap.end())
          {
            std::string colorCode = AnsiColors::colorMap_strMap[color];
            int segmentLength = end - prevEnd;
            std::string segment = currentLine.substr(prevEnd, segmentLength);

            coloredLine += AnsiColors::colorFormat(segment, colorCode);
            prevEnd = end;
          }
          else if (color.find("rgb") != std::string::npos)
          {
            // Extract RGB values
            std::regex rgbPattern("rgb\\((\\d{1,3}),(\\d{1,3}),(\\d{1,3})\\)");
            std::smatch rgbMatches;
            std::string colorCode;
            if (std::regex_search(color, rgbMatches, rgbPattern) && rgbMatches.size() == 4)
            {
              int r = std::stoi(rgbMatches[1].str());
              int g = std::stoi(rgbMatches[2].str());
              int b = std::stoi(rgbMatches[3].str());
              colorCode = AnsiColors::generate_rgb_sequence(r, g, b);
            }
            else
              Logger::logMessage("[Warning] [fileUtils/applyColorInstructions] Invalid RGB format in line " + std::to_string(i));
            colorCode = Ansi.fg.default_color;
            int segmentLength = end - prevEnd;
            std::string segment = currentLine.substr(prevEnd, segmentLength);
            coloredLine += AnsiColors::colorFormat(segment, colorCode);
            prevEnd = end;
          }
          else
            Logger::logMessage("[Warning] [fileUtils/applyColorInstructions] Invalid color code provided in the input file in line " + std::to_string(i));
        }
        // Append any remaining part of the line
        if (prevEnd < currentLine.length())
        {
          coloredLine += currentLine.substr(prevEnd);
        }
      }
      else
      {
        coloredLine = currentLine; // No color instructions for this line
      }
      coloredArtLines.push_back(coloredLine);
    }
    return coloredArtLines;
  }

  _map_data readAsciiArt(const std::string &fileName, const std::string &artName)
  {
    std::vector<std::string> currentartLines, currentColorLines, detailLines;
    std::ifstream file(artFilePath + fileName);
    bool artFound = artName.empty(), artSection = false, colorSection = false, detailSection = false;
    bool searchAll = artFound;

    _map_data map;

    if (!file)
    {
      Logger::logMessage("[ERROR] [fileUtils/readAsciiArt] Failed to open file: " + artFilePath + fileName);
      std::cerr << "[Error] Failed to open file: " << artFilePath + fileName << '\n';
      return map; // Return empty vectors as an indication of failure
    }

    std::string line;
    while (getline(file, line))
    {
      std::string trimmedLine = rtrim(line);
      // Parsing control logic based on markers
      if (trimmedLine.empty() || trimmedLine[0] == '#')
      {
        continue;
      }

      if (searchAll && trimmedLine.size() > 15 && trimmedLine.rfind("---Artname: ", 0) == 0 && trimmedLine.back() == '-')
      {
        std::string name = trimmedLine.substr(12, trimmedLine.size() - 15);
        map.artNames.push_back(name);
      }
      else if (!searchAll && trimmedLine == "---Artname: " + artName + "---")
      {
        artFound = true;
        continue;
      }
      else if (trimmedLine == "---ArtStart---" && artFound && !searchAll)
      {
        if (colorSection || detailSection)
        {
          std::string section = (colorSection ? std::string("colorSection ") : "") + (detailSection ? std::string("detailSection ") : "");
          Logger::logMessage("[Warning] [fileUtils/readAsciiArt] Art instructions found after " + section + fileName);
          colorSection = false;
          detailSection = false;
        }

        artSection = true;
        if (!currentartLines.empty())
        {
          Logger::logMessage("[Warning] [fileUtils/readAsciiArt] Art instructions found again after artStart " + fileName);
          map.artLinesSets.push_back(currentartLines); // Save the current set of art line instructions
          currentartLines.clear();                     // Prepare for a new set of art line instructions
        }
        continue;
      }
      else if (trimmedLine == "---ArtEnd---" && artFound && !searchAll)
      {
        artSection = false;
        if (!currentartLines.empty())
        {
          map.artLinesSets.push_back(currentartLines); // Save the current set of art line instructions
          currentartLines.clear();                     // Prepare for a new set of art line instructions
        }
        continue;
      }
      else if (trimmedLine == "---ColorStart---" && artFound && !searchAll)
      {
        if (artSection || detailSection)
        {
          std::string section = (artSection ? std::string("artSection ") : std::string("")) + (detailSection ? std::string("detailSection ") : std::string(""));
          Logger::logMessage("[Warning] [fileUtils/readAsciiArt] Color instructions found after " + section + fileName);
          artSection = false;
          detailSection = false;
        }

        colorSection = true;
        if (!currentColorLines.empty())
        {
          Logger::logMessage("[Warning] [fileUtils/readAsciiArt] Color instructions found again after colorStart " + fileName);
          map.colorLinesSets.push_back(currentColorLines); // Save the current set of color instructions
          currentColorLines.clear();                       // Prepare for a new set of color instructions
        }
        continue;
      }
      else if (trimmedLine == "---ColorEnd---" && artFound && !searchAll)
      {
        colorSection = false;
        if (!currentColorLines.empty())
        {
          map.colorLinesSets.push_back(currentColorLines); // Save the current set of color instructions
          currentColorLines.clear();                       // Prepare for a new set of color instructions
        }
        continue;
      }
      else if (trimmedLine == "---DetailStart---" && artFound && !searchAll)
      {
        if (artSection || colorSection)
        {
          std::string section = (artSection ? std::string("artSection ") : "") + (colorSection ? std::string("colorSection ") : "");
          Logger::logMessage("[Warning] [fileUtils/readAsciiArt] Detail instructions found after " + section + fileName);
          artSection = false;
          colorSection = false;
        }

        detailSection = true;
        continue;
      }
      else if (trimmedLine == "---DetailEND---" && artFound && !searchAll)
      {
        detailSection = false;
        continue;
      }
      else if (trimmedLine == "---END---")
      {
        if (!currentartLines.empty())
        {
          map.artLinesSets.push_back(currentartLines); // Save the current set of art line instructions
          currentartLines.clear();
        }
        if (!currentColorLines.empty())
        {
          map.colorLinesSets.push_back(currentColorLines); // Save the current set of color instructions
          currentColorLines.clear();
        }

        if (artFound && !searchAll)
          break; // End of the file
        else
          continue;
      }

      // Parsing the content of the sections
      if (artSection)
      {
        currentartLines.push_back(line);
        // Logger::logMessage("[DEBUG] [fileUtils/readAsciiArt] artSection " + line);
      }
      else if (colorSection)
      {
        currentColorLines.push_back(line);
        // Logger::logMessage("[DEBUG] [fileUtils/readAsciiArt] colorSection " + line);
      }
      else if (detailSection)
      {
        if (line.rfind("player_startLocation=", 0) == 0)
        {
          std::string value = line.substr(21);
          size_t commaPos = value.find(',');
          if (commaPos != std::string::npos)
          {
            int x = std::stoi(value.substr(0, commaPos));
            int y = std::stoi(value.substr(commaPos + 1));
            map.detail.player_loc = std::make_pair(x, y);
          }
          else
          {
            Logger::logMessage("[ERROR] [fileUtils/readAsciiArt] Invalid detail line: " + line);
          }
        }
        else if (line.rfind("intro=", 0) == 0)
        {
          std::string content = line.substr(6); // Extract the content after the prefix
          std::istringstream iss(content);
          std::string part;

          // Use getline to parse the line based on commas
          while (std::getline(iss, part, ';'))
          {
            map.detail.intro.push_back(part);
          }
        }
        else if (line.rfind("enemyLocation=", 0) == 0)
        {
          std::string value = line.substr(14);
          size_t commaPos = value.find(',');
          if (commaPos != std::string::npos)
          {
            int x = std::stoi(value.substr(0, commaPos));
            int y = std::stoi(value.substr(commaPos + 1));
            map.detail.enemy_locs.push_back(std::make_pair(x, y));
          }
          else
          {
            Logger::logMessage("[ERROR] [fileUtils/readAsciiArt] Invalid detail line: " + line);
          }
        }
        else
        {
          Logger::logMessage("[ERROR] [fileUtils/readAsciiArt] Invalid detail line: " + line);
        }
      }
    }
    file.close();

    bool end_error = false;
    std::string errorMessage = "[ERROR] [fileUtils/readAsciiArt] ---END--- marker not found in the file. The last set of art instructions (";
    if (!currentartLines.empty())
    {
      map.artLinesSets.push_back(currentartLines); // Save the current set of art line instructions
      errorMessage += " art line";
      end_error = true;
    }
    if (!currentColorLines.empty())
    {
      map.colorLinesSets.push_back(currentColorLines); // Save the current set of color instructions
      errorMessage += " color line";
      end_error = true;
    }

    if (end_error)
    {
      errorMessage += ") may be incomplete.";
      Logger::logMessage(errorMessage);
    }

    return map;
  }
} // namespace FileUtils
