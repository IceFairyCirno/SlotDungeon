// fileUtils.h

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>

namespace FileUtils
{
  typedef struct
  {
    std::pair<int, int> player_loc;
    std::vector<std::pair<int, int>> enemy_locs;
    std::vector<std::string> intro{};
    void clear()
    {
      intro.clear();         // Clear the vector of strings
      intro.shrink_to_fit(); // Reduce capacity to zero
                             // player_loc does not need special handling since it contains primitive types
      enemy_locs.clear();    // Clear the vector of integers
    }
  } _map_details;

  typedef struct
  {
    std::string error;
    std::vector<std::string> artNames;
    std::vector<std::vector<std::string>> artLinesSets;
    std::vector<std::vector<std::string>> colorLinesSets;
    _map_details detail;

    void clear()
    {
      error.clear();
      error.shrink_to_fit();

      artNames.clear();
      artNames.shrink_to_fit();

      for (auto &artLine : artLinesSets)
      {
        artLine.clear();
        artLine.shrink_to_fit();
      }
      artLinesSets.clear();
      artLinesSets.shrink_to_fit();

      for (auto &colorLine : colorLinesSets)
      {
        colorLine.clear();
        colorLine.shrink_to_fit();
      }
      colorLinesSets.clear();
      colorLinesSets.shrink_to_fit();

      detail.clear(); // Clear nested struct
    }
  } _map_data;

  // extern const _map_data mapData;

  std::vector<std::string> readLinesFromFile(const std::string &filePath);
  std::vector<std::string> applyColorInstructions(const std::vector<std::string> &artLines, const std::vector<std::string> &colorLines);
  _map_data readAsciiArt(const std::string &fileName, const std::string &artName = "");
}

#endif // FILE_UTILS_H