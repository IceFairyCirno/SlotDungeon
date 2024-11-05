#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "commonUtils.h"
#include "fileUtils.h"
#include "settings.h"

const std::string setting_filename = "config.ini";

namespace GameSettings
{
  // Function to load an INI file
  bool SettingsManager::loadIniFile()
  {
    auto temp_settingsMetadata = settingsMetadata;
    temp_settingsMetadata.erase("error");
    std::ifstream file(setting_filename);
    std::string line, currentSection;
    if (!file.good())
    {
      Logger::logMessage("[WARNING] [settings/loadIniFile] Creating missing file found: " + setting_filename);
      saveIniFile();
      return false;
    }
    if (!file.is_open())
    {
      std::cerr << "Unable to open file: " << setting_filename << std::endl;
      return false;
    }

    bool error = false;
    while (std::getline(file, line))
    {
      line = trim(line);
      if (line.empty() || line[0] == ';' || line[0] == '#')
      {
        // Skip empty lines and comments
        continue;
      }

      if (line[0] == ';')
      {
        continue;
      }
      else
      {
        // This is a key=value line
        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos)
        {
          Logger::logMessage("[ERROR] [settings/loadIniFile] Invalid key=value pair: " + line);
          error = true;
          continue;
        }
        std::string key = trim(line.substr(0, delimiterPos));
        std::string value = trim(line.substr(delimiterPos + 1));

        if (temp_settingsMetadata.erase(key))
        {

          if (key == "FPS")
          {
            try
            {
              Settings.FPS = std::stoi(value);
              Logger::logMessage("[DEBUG] [settings/validData] FPS = " + value);
            }
            catch (const std::invalid_argument &e)
            {
              Settings.error = true;
              Logger::logMessage("[ERROR] [settings/validData] Invalid integer value for FPS = " + value);
            }
          }
          else if (key == "SHOW_FPS")
          {
            try
            {
              std::stoi(value) ? Settings.SHOW_FPS = true : Settings.SHOW_FPS = false;
            }
            catch (const std::invalid_argument &e)
            {
              Settings.error = true;
              Logger::logMessage("[ERROR] [settings/validData] Invalid boolean value for SHOW_FPS = " + value);
            }
          }
          else if (key == "lastlevel")
          {
            try
            {
              Settings.lastlevel = std::stoi(value);
              Logger::logMessage("[DEBUG] [settings/validData] lastlevel = " + value);
            }
            catch (const std::invalid_argument &e)
            {
              Settings.error = true;
              Logger::logMessage("[ERROR] [settings/validData] Invalid integer value for lastlevel = " + value);
            }
          }
          else if (key == "playerName")
          {
            Settings.playerName = value;
          }
          else if (key == "menuArtVersion")
          {
            Settings.menuArtVersion = value;
          }
        }
        else
        {
          Logger::logMessage("[ERROR] [settings/loadIniFile] Invalid key: " + key);
          error = true;
        }
      }
    }

    if (!temp_settingsMetadata.empty() || Settings.error)
    {
      // Re save data
      Logger::logMessage("[WARNING] [settings/loadIniFile] Fixing config data.");
      std::string missing;
      for (const auto &meta : temp_settingsMetadata)
      {
        missing += meta + ", ";
      }
      Logger::logMessage("[WARNING] [settings/loadIniFile] Data missing: " + missing.substr(0, missing.size() - 2) + ".");
      if (saveIniFile())
      {
        Settings.error = false;
      }
    }
    Logger::logMessage("[DEBUG] [settings/loadIniFile] Settings loaded.");
    return Settings.error;
  }

  bool SettingsManager::saveIniFile()
  {
    std::ofstream file(setting_filename);
    if (!file.is_open())
    {
      std::cerr << "Unable to open file for writing: " << setting_filename << std::endl;
      return false;
    }

    // Write the key=value pairs in the specified order
    // auto temp_data_inOrder = data_inOrder;
    // while (!temp_data_inOrder.empty())
    // {
    //   std::string key = temp_data_inOrder.front();
    //   temp_data_inOrder.pop();
    for (const auto &key : settingsMetadata)
    {
      if (key == "error")
      {
        continue;
      }
      else if (key == "SHOW_FPS")
      {
        file << key << "=" << std::to_string(Settings.SHOW_FPS) << std::endl;
      }
      else if (key == "FPS")
      {
        file << key << "=" << Settings.FPS << std::endl;
      }
      else if (key == "lastlevel")
      {
        file << key << "=" << Settings.lastlevel << std::endl;
      }
      else if (key == "playerName")
      {
        file << key << "=" << Settings.playerName << std::endl;
      }
      else if (key == "menuArtVersion")
      {
        file << key << "=" << Settings.menuArtVersion << std::endl;
      }
    }
    // }

    file.close();
    Logger::logMessage("[DEBUG] [settings/saveIniFile] Settings saved.");
    return true;
  }

  void SettingsManager::load_map_list(std::vector<std::pair<std::string, bool>> &maps_list, bool force)
  {
    if (maps_list.empty() || force)
    {
      if (force)
      {
        Logger::logMessage("[INFO] [load_map_list] Reloading maps.");
        maps_list.clear();
      }
      Logger::logMessage("[INFO] [load_map_list] Loading maps.");
      auto maps_data = FileUtils::readAsciiArt("map.txt");
      for (const auto &i : maps_data.artNames)
      {
        Logger::logMessage("[DEBUG] [load_map_list] Adding map: " + i);
        maps_list.push_back(std::make_pair(i, true));
      }
    }
    Logger::logMessage("[DEBUG] [load_map_list] Maps loaded.");
  }

} // namespace GameSettings
