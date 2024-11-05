#include <iostream>
#include <sstream>
#include <vector>
#include <limits.h>

#include "game.h"
#include "gameData.h"
#include "commonUtils.h"
#include "menu.h"

const int max_gameMap_level = 4;
const bool __DEBUG = true;
const bool __DEBUG_game_map = true;

int printCWD()
{
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL)
  {
    std::string message = "[DEBUG] [printCWD] Current Working Directory: " + std::string(cwd);
    Logger::logMessage(message);
  }
  else
  {
    Logger::logMessage("[ERROR] [printCWD] getcwd() error");
    return 1;
  }
  return 0;
}

int main()
{
  Logger logger;
  logger.deleteOldLogs();
  Logger::logMessage("[DEBUG] [main] Log Initialized");

  TerminalUtils::setupTerminal();
  Logger::logMessage("[DEBUG] [main] Terminal Initialized");

  if (printCWD())
  {
    Logger::logMessage("[ERROR] [main] Unable to print cwd");
    // Error code 1
    return 1;
  }

  GameData::GameStats game_data = GameData::loadGameStats();
  if (game_data.error)
  {
    Logger::logMessage("[ERROR] [main] Game Data Load Error");
    // Error code 2
    return 2;
  }
  Logger::logMessage("[DEBUG] [main] Game Data Loaded");
  Logger::logMessage("[DEBUG] [main] Current Max Level: " + std::to_string(game_data.currentMaxLevel));

  GameSettings::SettingsManager settingsManager;
  if (settingsManager.loadIniFile())
  {
    Logger::logMessage("[ERROR] [main] Game Settings Load Error");
    std::cout << "Error loading settings file: " << std::endl;
    std::cout << "Do you want to reset the settings (Y/N)? " << std::endl;
    int key;
    do
    {
      key = TerminalUtils::readKey();
    } while (key == 0 || (key != 'N' && key != 'n' && key != 'Y' && key != 'y'));
    if (key == 'Y' || key == 'y')
    {
      settingsManager.saveIniFile();
      if (settingsManager.loadIniFile())
      {
        Logger::logMessage("[ERROR] [main] Game Settings Load Error");
        // Error code 3
        return 3;
      }
    }
    else
    {
      return 2;
    }
  }
  Logger::logMessage("[DEBUG] [main] Game Settings Loaded");

  // Prevent changing config file to pass the level
  if (settingsManager.Settings.lastlevel > game_data.currentMaxLevel)
  {
    settingsManager.Settings.lastlevel = game_data.currentMaxLevel;
    settingsManager.saveIniFile();
  }

  std::vector<std::string> exit_items{
      AnsiColors::colorFormat("Game Exit", Ansi.fg.red, Ansi.font.bold),
      "",
      "",
      "",
      "",
      AnsiColors::colorFormat("Credits:", Ansi.fg.bblue, Ansi.font.bold),
      "",
      "1340 Group 35",
      "",
      "",
      "",
      "©2024 ENGG 1340 Group 35",
      "",
      "",
      "",
      AnsiColors::colorFormat("See you next time!", Ansi.fg.bgreen, Ansi.font.flash),
  };

  Logger::logMessage("[DEBUG] [main] Initialize Menu");
  GameMenu::Menu menu(settingsManager, game_data); // Instantiate Menu

  // Provide detailed feedback based on initialization success
  if (!menu.isError())
  {
    Logger::logMessage("[DEBUG] [main] Menu initialized successfully. Proceeding with application flow.");
  }
  else
  {
    Logger::logMessage("[ERROR] [main] Failed to initialize menu. Check configuration and menu assets.");
    // Error code 4
    return 4;
  }

  // Inform about the transition to running the menu
  int result;
  std::string errorMessage = "";

  // Recall game
  bool recall_game = false;

  Logger::logMessage("[DEBUG] [main] Running menu selection interface.");
  menu.newPlayer = game_data.currentMaxLevel == 0;
  menu.max_gameMap_level = max_gameMap_level;
  Logger::logMessage("[DEBUG] [main] New Player: " + std::to_string(menu.newPlayer));
  do
  {
    if (recall_game)
    {
      Logger::logMessage("[INFO] [main] Recall Game");
      menu.newPlayer = false;
      menu.gameMap_name = "Default_Map" + std::to_string(settingsManager.Settings.lastlevel);
      result = 1;
    }
    else
    {
      if (!errorMessage.empty())
      {
        Logger::logMessage("[INFO] [main] Menu Error: " + errorMessage);
      }
      result = menu.run(errorMessage); // Operate the Menu
      Logger::logMessage("[INFO] [main] Menu Exit with Code " + std::to_string(result) + ".");
      Logger::logMessage("[DEBUG] [main] settingsManager.Settings.playerName = " + settingsManager.Settings.playerName + ".");
    }

    // Switch-case can be expanded with specific cases for better clarity
    switch (result)
    {
    // Quit the game
    case 0:
    {
      Logger::logMessage("[INFO] [main] User selected Quit from Main Menu. Exiting Application.");
      system("clear");

      TerminalUtils::printFrame(exit_items);
      errorMessage.empty() ? Logger::logMessage("[INFO] [main] User exited game from menu. No found errors reported.")
                           : Logger::logMessage("[WARNING] [main] User exited game from menu with following warning (" + errorMessage + ").");
      break;
    }

    // Start the game
    case 1:
    {
      // Determine the game map to load
      std::string gameMap_name;
      if (menu.gameMap_name.empty() || recall_game)
      {
        recall_game = false;

        if (settingsManager.Settings.lastlevel >= max_gameMap_level)
        {
          Logger::logMessage("[INFO] [main] User reached maximum level. Transitioning to first map.");
          gameMap_name = "Default_Map1";
          settingsManager.Settings.lastlevel = 1;
          settingsManager.saveIniFile();

          // game_data = GameData::loadGameStats();
        }
        else
        {
          if (settingsManager.Settings.lastlevel == 0)
          {
            Logger::logMessage("[INFO] [main] User is a new player. Transitioning to first map.");
            gameMap_name = "Default_Map1";
            settingsManager.Settings.lastlevel = 1;
            settingsManager.saveIniFile();
            if (settingsManager.Settings.lastlevel > game_data.currentMaxLevel)
            {
              game_data.currentMaxLevel = settingsManager.Settings.lastlevel;
              GameData::saveGameStats(game_data);
            }
          }
          else
          {
            Logger::logMessage("[INFO] [main] No game map specified. Loading last played map.");
            Logger::logMessage("[INFO] [main] Loading Default_Map" + std::to_string(settingsManager.Settings.lastlevel) + ".");
            gameMap_name = "Default_Map" + std::to_string(settingsManager.Settings.lastlevel);
          }
        }
      }
      else
      {
        Logger::logMessage("[INFO] [main] Game map specified. Loading " + menu.gameMap_name + ".");
        gameMap_name = menu.gameMap_name;
      }
      Logger::logMessage("[INFO] [main] User initiated game start. Transitioning to game environment.");

      MainGame::Game game(gameMap_name,
                          settingsManager,
                          game_data,
                          __DEBUG_game_map);
      if (game.isError())
      {
        // Logger::logMessage("[ERROR] [main] Failed to initialize game. Application exit.");
        Logger::logMessage("[FATAL] [main] Failed to initialize game.");
        errorMessage = "Failed to initialize game. Check log for more detail.";
        recall_game = false;
        // Error code 5
        // return 5;
      }
      else
      {
        Logger::logMessage("[DEBUG] [main] Game initialized successfully.");
        int gameReturn = game.start();
        switch (gameReturn)
        {
        case 0:
          Logger::logMessage("[INFO] [main] User exited game normally.");
          break;

        case 1:
          Logger::logMessage("[INFO] [main] User exited game with user exit.");
          break;

        case 2:
          Logger::logMessage("[INFO] [main] User exited game and continue.");
          // Prevent cheating
          if (settingsManager.Settings.lastlevel > game_data.currentMaxLevel)
          {
            Logger::logMessage("[WARNING] [main] User tried to cheat by skipping levels. Resetting to current level.");
            Logger::logMessage("[INFO] [main] Setting lastlevel from " + std::to_string(settingsManager.Settings.lastlevel) + ".");
            settingsManager.Settings.lastlevel = game_data.currentMaxLevel;
            Logger::logMessage("[INFO] [main] Setting lastlevel to " + std::to_string(settingsManager.Settings.lastlevel) + ".");
            Logger::logMessage("[INFO] [main] game_data.currentMaxLevel:" + std::to_string(game_data.currentMaxLevel));
          }

          // Check if the player reached the maximum level
          if (settingsManager.Settings.lastlevel > max_gameMap_level)
          {
            Logger::logMessage("[INFO] [main] User reached maximum level. Return to menu.");
          }
          else
          {
            // Add level
            settingsManager.Settings.lastlevel += 1;
            settingsManager.saveIniFile();
            // settingsManager.loadIniFile();
            // Logger::logMessage("[INFO] [main] Setting lastlevel to " + std::to_string(settingsManager.Settings.lastlevel) + ".");

            // Check if the player passed a new level
            if (settingsManager.Settings.lastlevel > game_data.currentMaxLevel)
            {
              game_data.currentMaxLevel = settingsManager.Settings.lastlevel;
            }
            GameData::saveGameStats(game_data);
            if (settingsManager.Settings.lastlevel < max_gameMap_level)
            {
              recall_game = true;
            }
          }

          break;

        default:
          break;
        }
      }
      break;
    }

    // Error handling for menu operation
    case 2:
      Logger::logMessage("[ERROR] [main] Failed to load necessary game file(s) during menu operation. Check game assets and paths.");
      break;

    default:
      // For unexpected cases, include the 'result' value for easier debugging
      Logger::logMessage("[WARNING] [main] Encountered unexpected menu result code: " + std::to_string(result) + ". Investigate potential unhandled cases.");
      break;
    }
  } while (result == 1 || result == 2);
  return result;
}
