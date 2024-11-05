#ifndef SETTINGS_H
#define SETTINGS_H

#include <unordered_set>
#include <set>
#include <variant>
#include <string>
#include <vector>
#include <map>
#include <queue>

namespace GameSettings
{

  struct setting
  {
    bool error = false;
    bool SHOW_FPS = true;
    int FPS = 60;
    int lastlevel = 0;
    std::string playerName = "Player foo";
    std::string menuArtVersion = "Menu0";
  };

  class SettingsManager
  {
  private:
    std::queue<std::string> data_inOrder;
    std::set<std::string> settingsMetadata{
        "error",
        "SHOW_FPS",
        "FPS",
        "lastlevel",
        "playerName",
        "menuArtVersion",
    };

  public:
    setting Settings;

    SettingsManager()
    {
      // Initialize settings with default values
      for (const auto &meta : settingsMetadata)
      {
        data_inOrder.push(meta);
      }
    }

    bool loadIniFile();
    bool saveIniFile();
    // Check if the data is valid
    // For example, check if all required keys are present
    // and if the values are of the correct type
    std::map<std::string, std::string> validData(std::map<std::string, std::string> initData);

    // Load list of maps if maps_list is empty
    // force if needed to reload the list
    void load_map_list(std::vector<std::pair<std::string, bool>> &maps_list, bool force = false);
  };

} // namespace GameSettings

#endif // SETTINGS_H
