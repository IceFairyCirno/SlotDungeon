#include <iostream>
#include <limits>
#include <unistd.h>
#include <string>
#include <chrono>
#include <thread>
#include <numeric>

#include "menu.h"
#include "commonUtils.h"
#include "settings.h"
#include "fileUtils.h"
#include "gameData.h"

bool SHOW_FPS = true;

namespace GameMenu
{
  std::string menu_select_color = Ansi.fg.yellow;
  std::string menu_selected_color = Ansi.fg.green;
  int MaxMenuSelection = 4;
  std::vector<bool> settingsChoice_pressable = {
      true,  // Player Name open box and enter name
      true,  // FPS: change after press
      false, // Menu Art Version show menu art version directly
  };

  Menu::Menu(GameSettings::SettingsManager &settingsManager,
             GameData::GameStats &game_data) : settingsManager(settingsManager),
                                               game_data(game_data),
                                               current_choice(1),
                                               lastRefreshTime(std::chrono::steady_clock::now()),   // Initialize last refresh interval
                                               lastArtRefreshTime(std::chrono::steady_clock::now()) // Initialize last art refresh interval
  {
    TerminalUtils::TerminalSize originalTerminalSize = TerminalUtils::getTerminalSize();

    Logger::logMessage("[DEBUG] [GameMenu/init] Start load settings");
    menu_art_version = settingsManager.Settings.menuArtVersion;

    SHOW_FPS = settingsManager.Settings.SHOW_FPS;
    Logger::logMessage("[DEBUG] [GameMenu/init] Show FPS: " + std::to_string(SHOW_FPS));

    setFPS();
    Logger::logMessage("[DEBUG] [GameMenu/init] Menu FPS: " + std::to_string(FPS));

    setArtFPS();
    Logger::logMessage("[DEBUG] [GameMenu/init] Menu Art FPS: " + std::to_string(FPSArt));

    int loadMenuState = loadMenu();
    if (loadMenuState)
    {
      Logger::logMessage("[ERROR] [GameMenu/init] Exit with error [" + std::to_string(loadMenuState) + "] in menu.");
      initializationError = true;
      return;
    }

    menu_int = 0;
    Logger::logMessage("[DEBUG] [GameMenu/init] Menu Art Initialized");
  }

  void Menu::setFPS(int forceFPS)
  {
    if (forceFPS < 5)
    {
      forceFPS = std::max(settingsManager.Settings.FPS, 5); // Use setting and minimum 5
    }
    FPS = forceFPS;
    fpsToUs = 1000000.0 / FPS; // Convert FPS to microseconds per frame
    refreshInterval = std::chrono::microseconds(static_cast<int>(fpsToUs));
  }

  void Menu::setArtFPS(int artFPS)
  {
    if (artFPS < 3)
    {
      artFPS = 3; // Default to 3 or use a configured default
    }
    fpsArtToUs = 1000000.0 / artFPS; // Convert Art FPS to microseconds per frame
    refreshArtInterval = std::chrono::microseconds(static_cast<int>(fpsArtToUs));
  }

  int Menu::loadMenu()
  {
    Logger::logMessage("[DEBUG] [GameMenu/init] Loading menu art: " + menu_art_version);
    // Attempt to read the ASCII art and corresponding color sets
    auto artAndColors = FileUtils::readAsciiArt("menu.txt", menu_art_version);

    artTxtSets.clear();
    artColorSets.clear();
    artTxtSets = artAndColors.artLinesSets;
    artColorSets = artAndColors.colorLinesSets;

    if (!artAndColors.error.empty())
    {
      Logger::logMessage("[FATAL] [GameMenu] Error loading menu art: " + artAndColors.error);
      return 1;
    }

    if (artTxtSets.empty())
    {
      Logger::logMessage("[FATAL] [GameMenu] Menu art '" + menu_art_version + "' not found!");
      return 1;
    }

    Logger::logMessage("[DEBUG] [GameMenu/init] Menu art '" + menu_art_version + "' loaded successfully.");

    // Determine the difference in size between art text sets and color sets
    int diff = artTxtSets.size() - artColorSets.size();

    // Handle discrepancies between art frames and color frames
    if (diff != 0 && artColorSets.size() != 0)
    {
      std::string logMsg = "[WARNING] [GameMenu/init] '" + menu_art_version + "' has ";
      logMsg += (diff < 0) ? "less art frames than color frames. Duplicating last art frame to cover the color frames."
                           : "more art frames than color frames. Duplicating last color frame.";
      Logger::logMessage(logMsg + " Txt: " + std::to_string(artTxtSets.size()) + " color: " + std::to_string(artColorSets.size()));

      // Duplicate the last frame of the smaller set to match the size of the larger set
      auto &toExtend = (diff < 0) ? artTxtSets : artColorSets;
      for (int i = 0; i < std::abs(diff); ++i)
      {
        toExtend.push_back(toExtend.back());
      }
    }

    Logger::logMessage("[DEBUG] [GameMenu/init] Menu art '" + menu_art_version + "' check successfully.");

    // Set the maximum menu frame
    max_menuFrame = std::max(artTxtSets.size(), artColorSets.size());

    // Apply color to the initial frame
    if (artColorSets.size() != 0)
    {
      Logger::logMessage("[DEBUG] [GameMenu/init] Apply color to menu art ");
      artPieces = FileUtils::applyColorInstructions(artTxtSets[0], artColorSets[0]);
    }
    else
    {
      artPieces = artTxtSets[0];
    }

    return 0;
  }

  void Menu::displayUserNameConfirmationFrame(const std::string &user_input_name)
  {

    int contentWidth = 44; // The width inside the hashes
    int nameLength = static_cast<int>(user_input_name.size());
    int paddingLeft = (contentWidth - nameLength) / 2;
    int paddingRight = contentWidth - nameLength - paddingLeft;

    std::vector<std::string> frame = {
        std::string(48, '#'),
        "##                                            ##",
        "##                  Confirm?                  ##",
        "##                                            ##",
        "##                You entered:                ##",
        "##                                            ##",
        "##" + std::string(paddingLeft, ' ') + user_input_name + std::string(paddingRight, ' ') + "##",
        "##                                            ##",
        "##              Enter to Confirm              ##",
        "##               R/r to rewrite               ##",
        "##               ESC to Discard               ##",
        "##                                            ##",
        "##      You can later change in settings      ##",
        "##                                            ##",
        std::string(48, '#'),
    };

    TerminalUtils::printFrame(frame);
  }

  void Menu::UserNameInputFrame(const std::string &user_input_name)
  {
    int contentWidth = 42; // The width inside the hashes

    int userNameLength = static_cast<int>(user_input_name.size());
    int userPadLeft = (contentWidth - userNameLength) / 2;
    int userPadRight = contentWidth - userNameLength - userPadLeft;

    std::vector<std::string> staticFrame = {
        std::string(46, '#'),
        "##                                          ##",
        "##        Enter your new name below:        ##",
        "##                                          ##",
        "##       Max length 20 characters           ##",
        "##                                          ##",
        "##" + std::string(userPadLeft, ' ') + user_input_name + std::string(userPadRight, ' ') + "##",
        "##                                          ##",
        "##             Enter to Confirm             ##",
        "##                                          ##",
        std::string(46, '#'),
    };

    TerminalUtils::printFrame(staticFrame);
    auto T_size = TerminalUtils::getTerminalSize();
    TerminalUtils::moveCursorTo((T_size.rows - staticFrame.size()) / 2 + 6, (T_size.cols - staticFrame[0].size()) / 2 + 2 + user_input_name.size() + userPadLeft + 1);
  }

  std::string Menu::askUserForName()
  {
    std::string user_input_name = settingsManager.Settings.playerName;
    int key;
    do
    {
      system("clear"); // clear screen
      ansi(Ansi.cursor.show);
      std::cout.flush();
      do
      {
        UserNameInputFrame(user_input_name); // Show static frame with user input
        do
        {
          key = TerminalUtils::readKey(); // Read a single key input
        } while (key == 0);

        if (key == TerminalUtils::TerminalKeys::BACKSPACE && !user_input_name.empty())
        {
          user_input_name.pop_back();
        }
        else if (key == TerminalUtils::TerminalKeys::ENTER)
        {
          break;
        }
        else if (key == TerminalUtils::TerminalKeys::ESC)
        {
          ansi(Ansi.cursor.hide);
          std::cout.flush();
          return settingsManager.Settings.playerName;
        }
        else if (isprint(key) && user_input_name.size() < 20)
        {
          user_input_name.push_back(static_cast<char>(key));
        }

      } while (key != TerminalUtils::TerminalKeys::ENTER && key != TerminalUtils::TerminalKeys::ESC);
      ansi(Ansi.cursor.hide);
      std::cout.flush();
      
      key = 0;
      displayUserNameConfirmationFrame(user_input_name); // Show dynamically updated frame
      do
      {
        key = TerminalUtils::readKey(); // Read a single key input
      } while (key == 0);

    } while (key != TerminalUtils::TerminalKeys::ENTER && key != TerminalUtils::TerminalKeys::ESC);
    if (key != TerminalUtils::TerminalKeys::ESC)
    {
      return user_input_name; // Save the entered name
    }
    return settingsManager.Settings.playerName; // Return the original name
  }

  bool Menu::isError() const
  {
    return initializationError; // Method to check if an error occurred during initialization
  }

  void Menu::displayMenu()
  {
    if (TerminalUtils::getTerminalSize() != originalTerminalSize)
    {
      originalTerminalSize = TerminalUtils::getTerminalSize();
      system("clear");
    }

    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastRefreshTime);
    auto elapsedArtTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastArtRefreshTime);

    if (elapsedTime >= refreshInterval)
    {
      lastRefreshTime = currentTime; // Update the last refresh time

      if (SHOW_FPS)
      {
        // Add current frame time to the list
        frameTimes.push_back(elapsedTime.count());
        if (frameTimes.size() > maxFrameTimes)
        {
          frameTimes.erase(frameTimes.begin()); // Maintain the size limit
        }

        // Calculate average frame time if the vector is not empty
        double averageFrameTime = 0.0;
        if (!frameTimes.empty())
        {
          averageFrameTime = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0) / frameTimes.size();
        }

        double actualFPS = 0.0;
        if (averageFrameTime > 0)
        {
          actualFPS = 1e6 / averageFrameTime; // Calculate average FPS from microseconds
        }

        std::string sta = "FPS: " + std::to_string(static_cast<int>(actualFPS));
        TerminalUtils::printText(sta, 1, -3);
      }

      // Refresh art if needed
      if (elapsedArtTimeUs >= refreshArtInterval)
      {
        lastArtRefreshTime = currentTime; // Reset the timer

        if (artTxtSets.size() > 1 && artColorSets.size() > 0)
        {
          menuFrame = (menuFrame >= max_menuFrame - 1) ? 0 : menuFrame + 1;
          artPieces = FileUtils::applyColorInstructions(artTxtSets[menuFrame], artColorSets[menuFrame]);
        }
      }

      menu_items.clear();
      menu_items = artPieces;
      menu_items.insert(menu_items.end(), 2, ""); // Add space
      long long num;
      switch (menu_int)
      {
      // Main menu
      case 0:
      {
        menu_items.reserve(artPieces.size() + 7); // Reserve extra space for additional elements to avoid reallocations

        // Conditional content based on errorMessage and current_choice
        if (!errorMessage.empty())
        {
          if (!newPlayer)
          {
            menu_items.push_back("Welcome back, " + settingsManager.Settings.playerName + "!");
            menu_items.push_back("");
          }

          menu_items.push_back(current_choice == 1 ? AnsiColors::colorFormat(">| Unable to start game |<", menu_select_color)
                                                   : "   Unable to start game   ");
          if (!newPlayer)
          {
            menu_items.push_back(current_choice == 2 ? AnsiColors::colorFormat(">> Select Map <<", menu_select_color) : "   Select Map   ");
          }
        }
        else if (newPlayer)
        {
          menu_items.push_back(current_choice == 1 ? AnsiColors::colorFormat(">> Start Game <<", menu_select_color)
                                                   : "   Start Game   ");
        }
        else
        {
          menu_items.push_back("Welcome back, " + settingsManager.Settings.playerName + "!");
          menu_items.push_back("");
          if (settingsManager.Settings.lastlevel >= max_gameMap_level)
          {
            menu_items.push_back(current_choice == 1 ? AnsiColors::colorFormat(">> Restart Game <<", menu_select_color)
                                                     : "   Restart Game   ");
          }
          else
          {
            menu_items.push_back(current_choice == 1 ? AnsiColors::colorFormat(">> Continue <<", menu_select_color)
                                                     : "   Continue   ");
          }
          menu_items.push_back(current_choice == 2 ? AnsiColors::colorFormat(">> Select Map <<", menu_select_color) : "   Select Map   ");
        }

        menu_items.push_back(current_choice == 3 ? AnsiColors::colorFormat(">> Settings <<", menu_select_color) : "   Settings   ");
        menu_items.push_back(current_choice == 4 ? AnsiColors::colorFormat(">> Exit <<", menu_select_color) : "   Exit   ");

        // Error message, if any
        if (!errorMessage.empty())
        {
          menu_items.push_back("");
          menu_items.push_back("Error: " + errorMessage);
        }

        TerminalUtils::printFrame(menu_items);
        break;
      }

      // Select map
      case 1:
      {
        menu_items.reserve(artPieces.size() + 5 + maps_list.size()); // Reserve extra space for additional elements to avoid reallocations
        menu_items.push_back("===================== Select a map =====================");
        for (int i = 0; i < maps_list.size(); i++)
        {
          if (maps_list[i].second)
          {
            menu_items.push_back(current_choice == i + 1 ? ">> " + maps_list[i].first + " <<" : ("   " + maps_list[i].first + "   "));
          }
          else
          {
            menu_items.push_back(AnsiColors::colorFormat(maps_list[i].first + " (Locked)", Ansi.fg.red));
          }
        }
        menu_items.push_back(current_choice == maps_list.size() + 1 ? ">> Return to main menu <<" : "   Return to main menu   ");
        menu_items.push_back("========================================================");

        TerminalUtils::printFrame(menu_items);

        // TerminalUtils::printText("current choice: " + std::to_string(current_choice), 2, -2, true);
        break;
      }

      // Settings
      case 2:
      {
        settingsMenuChoices = {
            "Player Name: " + settingsManager.Settings.playerName,
            "FPS: " + std::to_string(settingsManager.Settings.FPS),
            "Display FPS: " + (settingsManager.Settings.SHOW_FPS ? std::string("On ") : std::string("Off")),
            "Menu Art Version: " + menu_art_version,
            "Save",
            "Discard",
        };

        menu_items.reserve(artPieces.size() + 9);
        menu_items.push_back("Settings");
        menu_items.push_back("");

        for (int i = 0; i < settingsMenuChoices.size(); i++)
        {
          menu_items.push_back(current_choice == i + 1 ? AnsiColors::colorFormat(">> " + settingsMenuChoices[i] + " <<", menu_select_color) : "   " + settingsMenuChoices[i] + "   ");
        }

        TerminalUtils::printFrame(menu_items);

        std::vector<std::string> current_tips;
        current_tips.reserve(8);
        current_tips.push_back(tips[0]);
        current_tips.push_back(tips[1]);

        // Add choice-specific tips
        switch (current_choice)
        {
        case 1:
          current_tips.push_back("Enter to change your name");
          break;

        case 2:
          current_tips.push_back("Ctrl + Right/Left +/- 1");
          current_tips.push_back("Right/Left +/- 10");
          current_tips.push_back("Shift + Right/Left +- 50");
          break;

        case 3:
          current_tips.push_back("Right/Left change menu art");
          current_tips.push_back("Enter to see the menu art");
          break;

        default:
          break;
        }

        for (int i = 2; i < 5; i++)
        {
          current_tips.push_back(tips[i]);
        }

        // Ensure there are at least 8 entries
        while (current_tips.size() < 8)
        {
          current_tips.insert(current_tips.begin(), "");
        }

        current_tips = TerminalUtils::alignTextToMaxLength(current_tips); // Make it same length to remove additional text

        for (int i = 0; i < 8; i++)
        {
          TerminalUtils::printText(current_tips[i], originalTerminalSize.rows - 8 + i, -3, false);
        }
      }
      default:
        break;
      }
      lastRefreshTime = std::chrono::steady_clock::now();
    }
  }

  // default return 0
  int Menu::handleUserChoice(int force_n)
  {
    int start_rows;
    int rows;

    switch (menu_int)
    {
    // Main menu
    case 0:
    {
      // Force_n is used to force a choice, if needed
      if (force_n < 1 || force_n > MaxMenuSelection)
      {
        force_n = current_choice;
      }

      // Animation
      // Calculate the row position for displaying the selected menu item
      // with adjustments for new players and based on the current choice.
      TerminalUtils::TerminalSize ts = TerminalUtils::getTerminalSize();
      start_rows = (ts.rows - menu_items.size()) / 2;
      rows = std::max(start_rows, 0) + artPieces.size() + current_choice + (newPlayer ? 0 : 2) + (newPlayer && current_choice > 2 ? 0 : 1); // Ensure it's not negative
      auto printTxt = menu_items[artPieces.size() + current_choice + (newPlayer ? current_choice <= 2 : 3)];

      for (int i = 0; i < 5; i++)
      {
        TerminalUtils::printText(AnsiColors::colorFormat(AnsiColors::strippedText(printTxt),
                                                         i % 2 == 0 ? menu_selected_color : menu_select_color),
                                 rows, -2, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(50 + i * 10));
      }

      switch (force_n)
      {
      // Start game / Continue / Restart
      case 1:
      {
        if (!errorMessage.empty())
        {
          return 0;
          break;
        }
        else
        {
          Logger::logMessage("[INFO] [GameMenu/handleUserChoice] Starting game."); // Log game start
          if (newPlayer)
          {
            Logger::logMessage("[INFO] [GameMenu] New player detected. Asking for name.");
            settingsManager.Settings.playerName = askUserForName();
            if (!settingsManager.saveIniFile())
            {
              Logger::logMessage("[ERROR] [GameMenu/handleUserChoice] Error saving settings.");
              errorMessage = "Error saving settings.";
              exit(1);
            }
            newPlayer = false;
          }
          return 1;
        }
        break;
      }

      // Select map
      case 2:
      {
        system("clear");
        // std::cout << "\033[2J\033[H"; // Clear screen
        // std::cout.flush();
        // Load maps
        settingsManager.load_map_list(maps_list);
        if (maps_list.empty())
        {
          Logger::logMessage("[ERROR] [GameMenu/displayMenu] No maps load!");
          // return to main menu
          errorMessage = "No maps load!";
          break;
        }

        menu_int = 1;
        system("clear");                                                          // clear screen
        Logger::logMessage("[DEBUG] [GameMenu/handleUserChoice] Map list load."); // Log loading maps
        // current_choice = game_data.currentMaxLevel + 1;
        // Use settings version as to allow continue from last played level
        current_choice = std::max(settingsManager.Settings.lastlevel, 1);
        current_choice = std::min(current_choice, static_cast<int>(maps_list.size())); // Cast maps_list.size() to int

        for (int i = 0; i < maps_list.size(); i++)
        {
          if (maps_list[i].first == "Default_Map" + std::to_string(i + 1))
          {
            if (i + 1 > game_data.currentMaxLevel + 1)
            {
              maps_list[i].second = false;
            }
          }
          else
          {
            maps_list[i].second = true;
          }
        }
        // Change to -1 as 0 will be use by menu_int = 1
        return -1;
        break;
      }

      // Settings
      case 3:
      {
        Logger::logMessage("[DEBUG] [GameMenu/handleUserChoice] Opening settings."); // Log opening settings

        // Load nessary data
        menu_data = FileUtils::readAsciiArt("menu.txt");

        if (!menu_data.error.empty())
        {
          Logger::logMessage("[ERROR] [GameMenu/handleUserChoice] Error loading menu:  " + menu_data.error); // Log error loading map data
          errorMessage = "Error loading menu data.";
          break;
        }

        Logger::logMessage("[DEBUG] [GameMenu/handleUserChoice] Menu for settings loaded."); // Log settings loaded

        // Find the current menu art version
        auto it = find(menu_data.artNames.begin(), menu_data.artNames.end(), menu_art_version);
        if (it != menu_data.artNames.end())
        {
          current_choice = it - menu_data.artNames.begin();
        }
        else
        {
          Logger::logMessage("[ERROR] [GameMenu/handleUserChoice] Error loading menu:  " + menu_art_version + " not found."); // Log error loading map data
          errorMessage = "Error loading menu data.";
          break;
        }

        current_choice = 1; // Reset choice
        menu_int = 2;       // Change menu
        system("clear");    // clear screen
        return -1;

        break;
      }

      // Exit application
      case 4:
      {
        int key;
        do
        {
          std::vector<std::string> exit_items = {
              std::string(42, '#'),
              "##                                      ##",
              "##                                      ##",
              "##    Are you sure you want to exit?    ##",
              "##                                      ##",
              "##                                      ##",
              "##                                      ##",
              "##       " + AnsiColors::colorFormat("Yes (Y)", AnsiColors::colorMap.Red) + "          " + AnsiColors::colorFormat("No (N)", AnsiColors::colorMap.BGreen) + "        ##",
              "##                                      ##",
              "##                                      ##",
              std::string(42, '#'),
          };

          TerminalUtils::printFrame(exit_items);
          do
          {
            key = TerminalUtils::readKey();
          } while (key == 0);
        } while (key != 'N' && key != 'n' && key != 'Y' && key != 'y' && key != TerminalUtils::TerminalKeys::ESC);
        switch (key)
        {
        case 'Y':
        case 'y':
        {
          Logger::logMessage("[INFO] [GameMenu/handleUserChoice] User exited normally."); // Log normal exit
          return 2;                                                                       // Exit the program
          break;
        }
        default:
          Logger::logMessage("[INFO] [GameMenu/handleUserChoice] User returned to menu."); // Log return to menu
          break;
        }
        break;
      }

      default:
        std::cout << "Invalid choice. Bug in menu got " << force_n << ".\n";
        Logger::logMessage("[WARNING] [GameMenu/handleUserChoice] Invalid choice encountered: " + std::to_string(force_n)); // Log invalid choice
        std::cout << "Press enter to continue ...";
        std::cin.get();
        std::cout << '\n';
        break;
      }

      break;
    }

    // Select map
    case 1:
    {
      // Force_n is used to force a choice, if needed
      if (force_n < 1 || force_n > maps_list.size() + 1)
        force_n = current_choice;

      // Animation
      if (force_n >= 1 && force_n <= maps_list.size() + 1)
      {
        // Find the start rows for choice
        TerminalUtils::TerminalSize ts = TerminalUtils::getTerminalSize();
        start_rows = (ts.rows - menu_items.size()) / 2 + 1;
        rows = std::max(start_rows, 0) + artPieces.size() + 2 + current_choice - 1; // Ensure it's not negative
      }
      for (int i = 0; i < 5; i++)
      {
        TerminalUtils::printText(AnsiColors::colorFormat(
                                     AnsiColors::strippedText(menu_items[artPieces.size() + 2 + current_choice]),
                                     i % 2 == 0 ? menu_selected_color : menu_select_color),
                                 rows, -2, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(50 + i * 10));
      }
      // Animation End

      if (1 <= force_n && force_n <= game_data.currentMaxLevel + 1 && force_n <= maps_list.size())
      {
        // Set current open map
        Logger::logMessage("[DEBUG] [GameMenu/handleUserChoice] User selected map: " + maps_list[force_n - 1].first);
        settingsManager.Settings.lastlevel = force_n - 1;
        current_choice = 2;
        return 1;
      }
      else if (force_n == maps_list.size() + 1)
      {
        // Return to main menu
        Logger::logMessage("[DEBUG] [GameMenu/handleUserChoice] User returned to main menu.");
        current_choice = 2;
        return 0;
      }
      else
      {
        // Log invalid map selection
        Logger::logMessage("[ERROR] [GameMenu/handleUserChoice] Invalid map selection: " + std::to_string(force_n));
        current_choice = 2;
        return 2;
      }
      break;
    }

    // Settings
    case 2:
    {
      if (force_n < 1 || force_n > maps_list.size() + 1)
        force_n = current_choice;
      switch (force_n)
      {
      // Player Name
      case 1:
      {
        settingsManager.Settings.playerName = askUserForName(); // handle ask user for name and save to settings variable
        system("clear");  // clear screen
        return 0;
        break;
      }

      // FPS
      case 2:
      {
        setFPS(FPS); // Change menu FPS
        break;
      }

      // Display FPS
      case 3:
      {
        // Toggle display FPS
        SHOW_FPS = settingsManager.Settings.SHOW_FPS; // Toggle local FPS
        break;
      }

      // Menu Art Version
      case 4:
      {
        // Change variables
        Logger::logMessage("[DEBUG] [GameMenu/handleUserChoice] Changing menu art version to " + menu_art_version + "."); // Log changing menu art version

        if (loadMenu())
        {
          Logger::logMessage("[ERROR] [GameMenu/handleUserChoice] Error loading menu art."); // Log error loading menu art
          errorMessage = "Error loading menu art.";
          return 2;
        }

        break;
      }

      // Save and Exit
      case 5:
      {
        settingsManager.Settings.FPS = FPS;
        // Skip display FPS as it's already toggled
        settingsManager.Settings.menuArtVersion = menu_art_version;
        Logger::logMessage("[INFO] [GameMenu/handleUserChoice] Saving settings changes."); // Log saving settings changes
        settingsManager.saveIniFile();
        return 1;
        break;
      }

      // Discard and Exit
      case 6:
      {
        Logger::logMessage("[INFO] [GameMenu/handleUserChoice] Discarding settings changes."); // Log discarding settings changes
        return 2;
        break;
      }

      default:
        break;
      }
    }

    default:
      break;
    }

    system("clear");
    return 0;
  }

  // default return -1
  int Menu::selection()
  {
    int ch = TerminalUtils::readKey();

    if (ch == 0)
      return -1;
    switch (ch)
    {
    case TerminalUtils::TerminalKeys::UP:
    case 'w':
    case 'W':
    {
      switch (menu_int)
      {
      // Main menu
      case 0:
      {
        if (current_choice > 1)
        {
          if (current_choice == 3 && newPlayer)
            // Skip the map selection if new player
            current_choice -= 2;
          else
          {
            --current_choice;
          }
        }
        else
        {
          current_choice = MaxMenuSelection;
        }
        break;
      }

      // Map selection
      case 1:
      {
        if (current_choice > 1)
        {
          for (; current_choice > 1 && !maps_list[current_choice - 2].second; --current_choice)
            ;
          --current_choice;
        }

        else
        {
          // Return to main menu selection
          current_choice = maps_list.size() + 1;
        }
        break;
      }

      // Settings
      case 2:
      {
        if (current_choice >= 1)
        {
          --current_choice;
        }
        else
        {
          current_choice = settingsMenuChoices.size();
        }
        break;
      }
      default:
        break;
      }

      break;
    }

    case TerminalUtils::TerminalKeys::DOWN: // Down arrow
    case 's':
    case 'S':
    {
      switch (menu_int)
      {
      // Main menu
      case 0:
      {
        if (current_choice < MaxMenuSelection)
        {
          if (current_choice == 1 && newPlayer)
            // Skip the map selection if new player
            current_choice += 2;
          else
          {
            current_choice++;
          }
        }
        else
        {
          current_choice = 1;
        }
        break;
      }

      // Map selection
      case 1:
      {
        if (current_choice < maps_list.size() + 1)
        {
          for (; current_choice < maps_list.size() && !maps_list[current_choice].second; ++current_choice)
            ;
          ++current_choice;
          if (current_choice == maps_list.size() && !maps_list[current_choice - 1].second)
          {
            ++current_choice;
          }
        }
        else
        {
          current_choice = 1;
        }
        break;
      }

      // Settings
      case 2:
      {
        if (current_choice < settingsMenuChoices.size())
        {
          ++current_choice;
        }
        else
        {
          current_choice = 1;
        }
        break;
      }
      }
      break;
    }

    case TerminalUtils::TerminalKeys::RIGHT: // Right arrow
    case TerminalUtils::TerminalKeys::CTRL_RIGHT:
    case TerminalUtils::TerminalKeys::SHIFT_RIGHT:
    case 'd':
    case TerminalUtils::TerminalKeys::CTRL_D:
    case 'D':
    {
      switch (menu_int)
      {
      case 2:
        // Settings change variables
        switch (current_choice)
        {
        // FPS
        case 2:
        {
          int addFPS;
          switch (ch)
          {
          case TerminalUtils::TerminalKeys::CTRL_RIGHT:
          case TerminalUtils::TerminalKeys::CTRL_D:
            addFPS = 1;
            break;

          case TerminalUtils::TerminalKeys::SHIFT_RIGHT:
          case 'D':
            addFPS = 50;
            break;

          case TerminalUtils::TerminalKeys::RIGHT:
          case 'd':
            addFPS = 10;
            break;

          default:
            break;
          }
          FPS += addFPS;
          settingsManager.Settings.FPS = FPS;
          break;
        }

        // Display fps
        case 3:
        {
          settingsManager.Settings.SHOW_FPS = !settingsManager.Settings.SHOW_FPS;
          break;
        }

        // Menu Art Version
        case 4:
        {
          if (currentMenuArt >= menu_data.artNames.size())
          {
            currentMenuArt = 1;
          }
          else
          {
            ++currentMenuArt;
          }
          menu_art_version = menu_data.artNames[currentMenuArt - 1];
          break;
        }

        // Player Name
        default:
          break;
        }
        break;
      default:
        break;
      }
      break;
    }

    case TerminalUtils::TerminalKeys::LEFT: // Right arrow
    case TerminalUtils::TerminalKeys::CTRL_LEFT:
    case TerminalUtils::TerminalKeys::SHIFT_LEFT:
    case 'a':
    case TerminalUtils::TerminalKeys::CTRL_A:
    case 'A':
    {
      switch (menu_int)
      {
      case 2:
        // Settings change variables
        switch (current_choice)
        {
        // FPS
        case 2:
        {
          int addFPS;
          switch (ch)
          {
          case TerminalUtils::TerminalKeys::CTRL_LEFT:
          case TerminalUtils::TerminalKeys::CTRL_A:
            addFPS = 1;
            break;

          case TerminalUtils::TerminalKeys::SHIFT_LEFT:
          case 'A':
            addFPS = 50;
            break;

          case TerminalUtils::TerminalKeys::LEFT:
          case 'a':
            addFPS = 10;
            break;

          default:
            break;
          }
          FPS -= addFPS;
          FPS = std::max(5, FPS);
          settingsManager.Settings.FPS = FPS;
          break;
        }

        // Display fps
        case 3:
        {
          settingsManager.Settings.SHOW_FPS = !settingsManager.Settings.SHOW_FPS;
          break;
        }

        // Menu Art Version
        case 4:
        {
          if (currentMenuArt <= 1)
          {
            currentMenuArt = menu_data.artNames.size();
          }
          else
          {
            --currentMenuArt;
          }
          menu_art_version = menu_data.artNames[currentMenuArt - 1];
          break;
        }

        // Player Name
        default:
          break;
        }
        break;
      default:
        break;
      }
      break;
    }

    // case 13:                                 // \r, ASCII value 13 (window)
    case TerminalUtils::TerminalKeys::ENTER: // Linux enter key
    case TerminalUtils::TerminalKeys::SPACE: // Space key
    {
      int handleUCho = handleUserChoice();

      // Start or exit game
      switch (menu_int)
      {
      // Main menu
      case 0:
        if (handleUCho == 1 || handleUCho == 2)
        {
          return handleUCho;
        }
        break;

      // Select map
      case 1:
        if (handleUCho == 0 || handleUCho == 1 || handleUCho == 2)
        {
          return handleUCho;
        }
        break;

      // Settings
      case 2:
        if (handleUCho == 1 || handleUCho == 2)
        {
          return handleUCho;
        }

      default:
        break;
      }
      break;
    }

    case TerminalUtils::TerminalKeys::ESC: // ESC key
    {
      if (menu_int == 1)
      {
        return 0;
      }

      return 2; // Exit menu and application
      break;
    }

    default:
      return -1;
      break;
    }
    return -1;
  }

  int Menu::run(const std::string &errorMessage)
  {
    this->errorMessage = errorMessage;
    // std::cout << "\033[2J\033[H"; // Clear screen
    // std::cout.flush();            // Clear screen
    system("clear");
    gameMap_name.clear(); // Clear game map name

    int sel;
    bool break_loop = false;
    do
    {
      sel = selection(); // Non-blocking input check

      // Refresh display either on input
      displayMenu();

      // Check conditions to exit the loop
      switch (menu_int)
      {
      case 0:
      {
        // Start game
        if (sel == 1)
        {
          break_loop = true;
          break;
        }
        // Exit game
        else if (sel == 2)
        {
          sel = 0;
          break_loop = true;
          break;
        }
        break;
      }

      // Select map
      case 1:
      {
        if (sel == 0)
        {
          Logger::logMessage("[INFO] [GameMenu] User returned to main menu.");

          // Return to main menu
          menu_int = 0;
          current_choice = 2;
          sel = -1;
          system("clear");
          // std::cout << "\033[2J\033[H"; // Clear screen
          // std::cout.flush();
        }
        else if (sel == 1)
        {
          // Set current open map and open game
          Logger::logMessage("[INFO] [GameMenu] User selected map: " + maps_list[settingsManager.Settings.lastlevel].first);
          gameMap_name = maps_list[settingsManager.Settings.lastlevel].first;
          menu_int = 0;

          maps_list.clear(); // Free memory

          break_loop = true;
          break;
        }
        else if (sel == 2)
        {
          // Critical error
          Logger::logMessage("[ERROR] [GameMenu] Critical error encountered. Exiting application.");
          sel = 0;
          break_loop = true;
          break;
        }
        break;
      }

      // Settings
      case 2:
      {
        if (sel == 1 || sel == 2) // sel 2 is ESC
        {
          Logger::logMessage("[INFO] [GameMenu] User returned to main menu.");
          if (sel == 2)
          {
            Logger::logMessage("[INFO] [GameMenu] Reloding settings from file.");
            settingsManager.loadIniFile(); // Reload settings from file
          }
          FPS = settingsManager.Settings.FPS;                         // Reset menu FPS
          SHOW_FPS = settingsManager.Settings.SHOW_FPS;               // Reset display FPS
          menu_art_version = settingsManager.Settings.menuArtVersion; // Reset menu art
          setFPS();                                                   // Reset menu FPS
          loadMenu();                                                 // Reset menu art

          // Return to main menu
          menu_int = 0;
          current_choice = 3;

          menu_data.clear(); // Free memory

          // Clear screen
          system("clear");
          break;
        }
      }
      default:
        break;
      }

      if (break_loop) // Exit menu loop
        break;
    } while (true);

    Logger::logMessage("[INFO] [GameMenu] Game menu closed");
    return sel;
  }
} // namespace GameMenu
