// menu.h
#ifndef MENU_H
#define MENU_H

#include <chrono>
#include <unistd.h>
#include <vector>

#include "settings.h"
#include "gameData.h"
#include "commonUtils.h"
#include "fileUtils.h"

namespace GameMenu
{
  class Menu
  {
  private:
    // Show which menu selection is being displayed
    int menu_int;
    int current_choice;

    // FPS
    bool SHOW_FPS;
    int FPS;
    int FPSArt = 3;
    double fpsToUs;
    double fpsArtToUs = 1000000.0 / FPSArt;
    std::vector<double> frameTimes;
    const size_t maxFrameTimes = 10;

    // Error message to display
    std::string errorMessage;

    // initialize the menu
    std::string menu_art_version;

    // Settings and Data
    GameSettings::SettingsManager settingsManager;
    GameData::GameStats game_data;

    bool initializationError = false; // Flag to indicate initialization error

    std::vector<std::string> artPieces;
    std::vector<std::string> artTxt;
    std::vector<std::vector<std::string>> artTxtSets, artColorSets;
    int menuFrame = 0;
    int max_menuFrame;
    std::vector<std::string> menu_items;
    std::chrono::microseconds refreshInterval;
    std::chrono::microseconds refreshArtInterval;
    std::chrono::steady_clock::time_point lastRefreshTime, lastArtRefreshTime;

    std::vector<std::string> setting_menu_items;
    std::vector<std::pair<std::string, bool>> maps_list;
    std::vector<std::string> settingsMenuChoices;

    // Initialize default tips
    std::vector<std::string> tips = {
        std::string(30, '#'),
        "Use arrow keys to navigate",
        "Press Enter to select",
        "Press ESC to return",
        std::string(30, '#'),
    };

    FileUtils::_map_data map_data, menu_data;
    int originalMenuArt, currentMenuArt;

    // Set the FPS
    void setFPS(int forceFPS = -1);
    void setArtFPS(int artFPS = -1);

    // load menuArt for the menu
    // Using menu_art_version to load the correct menuArt
    int loadMenu();

    // Ask user name and save it in settingsManager.settings
    // Remember to call settingsManager.saveIniFile() to save the settings
    void UserNameInputFrame(const std::string &user_input_name);
    void displayUserNameConfirmationFrame(const std::string &user_input_name);
    std::string askUserForName();

  public:
    bool newPlayer;
    int max_gameMap_level;
    std::string gameMap_name = "";
    TerminalUtils::TerminalSize originalTerminalSize;

    // Initialize the menu
    Menu(GameSettings::SettingsManager &settingsManager,
         GameData::GameStats &game_data);
    // Check if an error occurred during initialization
    bool isError() const;
    // Function to display the menu
    void displayMenu();
    // Function to handle user choice i.e. when enter/space is pressed
    int handleUserChoice(int force_n = 0);
    // Function to change choice i.e. when up/down arrow is pressed
    int selection();
    // Function to run the menu
    int run(const std::string &errorMessage = "");
  };
}

#endif // MENU_H
