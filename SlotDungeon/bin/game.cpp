#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <sstream> // Add missing include for std::stringstream
#include <random>
#include <algorithm>

#include "gameData.h"
#include "settings.h"
#include "commonUtils.h"
#include "fileUtils.h"
#include "game.h"
#include "blackMarket.h"
#include "battle.h"
#include "slotMachine.h"

namespace MainGame
{
  Game::Game(
      const std::string &mapName,
      GameSettings::SettingsManager &settingsManager,
      GameData::GameStats &game_data,
      const bool &DEBUG) : map_name(mapName),
                           settingsManager(settingsManager),
                           game_data(game_data),
                           DEBUG(DEBUG),
                           lastRefreshTime(std::chrono::steady_clock::now()) // Initialize last refresh interval
  {
    // Initialize the game objects and settings
    system("clear");

    originalTerminalSize = TerminalUtils::getTerminalSize();

    SHOW_FPS = settingsManager.Settings.SHOW_FPS; // Get the debug FPS setting

    int loadMap_return = loadMap();
    if (loadMap_return)
    {
      Logger::logMessage("[Error] [game/init] Exit with error [" + std::to_string(loadMap_return) + "] in map.");
      initializationError = true;
      return;
    }

    FPS = settingsManager.Settings.FPS;                                     // Get the FPS from the settings
    fpsToUs = 1000000.0 / FPS;                                              // Convert FPS to microseconds per frame
    refreshInterval = std::chrono::microseconds(static_cast<int>(fpsToUs)); // Set the refresh interval
    Logger::logMessage("[DEBUG] [game/init] FPS set to " + std::to_string(FPS));
  }

  bool Game::isError() const
  {
    return initializationError; // Method to check if an error occurred during initialization
  }

  int Game::loadMap()
  {
    if (map_name.empty())
    {
      Logger::logMessage("[ERROR] [game/loadMap] No map name provided.");
      return 1;
    }
    Logger::logMessage("[INFO] [game/loadMap] Loading map " + map_name + " ...");

    auto mapAndColor = FileUtils::readAsciiArt("map.txt", map_name);
    mapTxtSet = mapAndColor.artLinesSets;
    mapColorSet = mapAndColor.colorLinesSets;
    mapDetail = mapAndColor.detail;

    if (mapTxtSet.empty())
    {
      Logger::logMessage("[ERROR] [game/loadMap] No map loaded.");
      return 2;
    }
    Logger::logMessage("[DEBUG] [game/loadMap] Map found.");

    // Color the map if color is available
    if (mapColorSet.empty())
    {
      Logger::logMessage("[WARNING] [game/loadMap] No map color.");
      currentMap = mapTxtSet[0];
    }
    else
    {
      currentMap = FileUtils::applyColorInstructions(mapTxtSet[0], mapColorSet[0]);
      Logger::logMessage("[DEBUG] [game/loadMap] Add color to map.");
    }

    // Set player start location
    playerLocation.col = mapDetail.player_loc.first;
    playerLocation.row = mapDetail.player_loc.second;

    int byteCol = TerminalUtils::getByteIndexFromVisualIndex(currentMap[playerLocation.row], playerLocation.col);
    playerLocation.byteCol = byteCol;
    currentMap[playerLocation.row][byteCol] = 'P';

    oldPlayerLocation = playerLocation;
    Logger::logMessage("[DEBUG] [game/loadMap] Player start location set to " + std::to_string(mapDetail.player_loc.first) + " " + std::to_string(mapDetail.player_loc.second));

    location temp_location;
    for (auto &i : mapDetail.enemy_locs)
    {
      temp_location.col = i.first;
      temp_location.row = i.second;
      temp_location.byteCol = TerminalUtils::getByteIndexFromVisualIndex(currentMap[i.second], i.first);
      currentMap[i.second] = currentMap[i.second].substr(0, temp_location.byteCol) + "👾" + currentMap[i.second].substr(temp_location.byteCol + 2);
      enemyLocation.push_back(temp_location);
    }

    Logger::logMessage("[DEBUG] [game/loadMap] Map successfully loaded!");
    return 0;
  }

  bool Game::askExitGame()
  {
    std::vector<std::string> exit_items = {
        std::string(42, '#'),
        "##                                      ##",
        "##             Are you sure             ##",
        "##     You want to return to menu?      ##",
        "##                                      ##",
        "##  (Unsaved progress will be wasted!)  ##",
        "##                                      ##",
        "##                                      ##",
        "##       " + AnsiColors::colorFormat("Yes (Y)", AnsiColors::colorMap.Red) + "          " + AnsiColors::colorFormat("No (N)", AnsiColors::colorMap.BGreen) + "        ##",
        "##                                      ##",
        "##                                      ##",
        std::string(42, '#'),
    };

    TerminalUtils::printFrame(exit_items);
    int key;
    do
    {
      do
      {
        key = TerminalUtils::readKey();
      } while (key == 0);

      if (key == 'Y' || key == 'y')
      {
        return true;
      }
      else if (key == 'N' || key == 'n' || key == TerminalUtils::TerminalKeys::ESC)
      {
        return false;
      }
    } while (key != 'Y' || key != 'y' || key != 'N' || key != 'n' || key != TerminalUtils::TerminalKeys::ESC);

    return false;
  }

  // Return if should it replace
  bool Game::interaction(int return_move)
  {
    if (return_move == collisionItems::slotMachine)
    {
      chooseStat(game_data.player_last_state.ATK, game_data.player_last_state.DEF, game_data.player_last_state.HP, game_data.player_last_state.coins);
      refreshScreen(true);
      GameData::saveGameStats(game_data);
    }
    else if (return_move == collisionItems::shop) // Shop
    {
      blackmarket(game_data);
      GameData::saveGameStats(game_data);
    }
    else if (return_move == collisionItems::enemy)
    {
      // initialize playerdata and enmey
      playerdata player;
      player.hp = game_data.player_last_state.HP;
      player.atk = game_data.player_last_state.ATK;
      player.def = game_data.player_last_state.DEF;
      player.mp = 0;
      player.skill1 = {std::max(int(player.hp * 0.25), 100), int(player.atk * 0.8), -5, 10};
      player.skill2 = {-(std::max(int(player.hp * 0.5), 100)), int(player.atk * 1.7), 15, 30};
      player.skill3 = {int(player.hp * 0.1), int(player.atk * 1.4), 10, -20};
      player.skill4 = {int(player.hp * 0.15), int(player.atk * 2.5), 10, -100};
      for (int i = 0; i < game_data.player_last_state.Inventory.size(); ++i)
      {
        for (int count = 0; count < game_data.player_last_state.Inventory[i]; ++count)
        {
          player.inventory.push_back(i + 1); // Adding 1 to make item indices 1-based
        }
      }
      int start, end;
      // get the battle screen (fight UI)
      std::vector<std::string> screen = FileUtils::readLinesFromFile("content/art/fightUI.txt");
      if (map_name == "Default_Map1")
      {
        start = 11;
        end = 18;
      }
      else if (map_name == "Default_Map2")
      {
        start = 21;
        end = 26;
      }
      else if (map_name == "Default_Map3")
      {
        start = 31;
        end = 32;
      }

      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> dis(start, end);
      std::string enemycode = std::to_string(dis(gen));

      while (enemyList.find(enemycode) == enemyList.end())
      { // while not find code in unorder map
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(start, end);
        enemycode = std::to_string(dis(gen));
      }

      bool firm = remindWindow(enemycode);
      Logger::logMessage("firm: " + std::to_string(firm));

      if (firm)
      {
        int reward = fight(player, enemycode, screen);
        if (reward != 0)
        {
          enemyList.erase(enemycode);
          game_data.player_last_state.coins += reward;
          game_data.player_last_state.Inventory = {
              static_cast<int>(std::count(player.inventory.begin(), player.inventory.end(), 1)),
              static_cast<int>(std::count(player.inventory.begin(), player.inventory.end(), 2)),
              static_cast<int>(std::count(player.inventory.begin(), player.inventory.end(), 3)),
              static_cast<int>(std::count(player.inventory.begin(), player.inventory.end(), 4))};

          GameData::saveGameStats(game_data);
          
          for (auto it = enemyLocation.begin(); it != enemyLocation.end();)
          {
            const auto &i = *it;
            if (std::abs(i.row - playerLocation.row) <= 2 && std::abs(i.col - playerLocation.col) <= 2)
            {
              std::string &mapRow = currentMap[i.row];
              mapRow = mapRow.substr(0, i.byteCol) + "  " + mapRow.substr(i.byteCol + 4);
              it = enemyLocation.erase(it); // Erase and move to the next element
              break;                        // Exit the loop if modification is done
            }
            else
            {
              ++it; // Only increment if no erasure has occurred
            }
          }
        }
      }
    }
    else if (return_move == collisionItems::door)
    {
      if (enemyLocation.empty())
      {
        return true;
      }
      else
      {
        TerminalUtils::printFrame(warningDoorLock);
        int key;
        do
        {
          key = TerminalUtils::readKey();
        } while (key == 0);
      }
    }
    refreshScreen(true);
    return false;
  }

  int Game::gameLoop()
  {
    Logger::logMessage("[DEBUG] [Game/Run] Game loop started.");
    int return_move;
    int key;
    do
    {
      key = TerminalUtils::readKey();
      switch (key)
      {
      case TerminalUtils::TerminalKeys::UP:
      case 'w':
      {
        return_move = movePlayer(playerLocation, currentMap, moveDirection::UP);
        switch (return_move)
        {
        case 0:
          playerMove_frame = true;
          break;
        case -1:
          //   Logger::logMessage("ERROR [gameLoop] Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Up");
          //   errorMsg = "Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Up";
          //   return -1;
          break;

        default:
          if (interaction(return_move))
          {
            return 2;
          }

          break;
        }
        break;
      }

      case TerminalUtils::TerminalKeys::SHIFT_UP:
      case 'W':
      {
        return_move = movePlayer(playerLocation, currentMap, moveDirection::UP, true);
        switch (return_move)
        {
        case 0:
          playerMove_frame = true;
          break;
        case -1:
          // Logger::logMessage("ERROR [gameLoop] Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Shift Up");
          // errorMsg = "Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Shift Up";
          // return -1;
          break;

        default:
          if (return_move >= 100)
          {
            playerMove_frame = true;
            return_move -= 100;
            refreshScreen();
          }
          if (interaction(return_move))
          {
            return 2;
          }

          break;
        }
        break;
      }

      case TerminalUtils::TerminalKeys::DOWN:
      case 's':
      {
        return_move = movePlayer(playerLocation, currentMap, moveDirection::DOWN);
        switch (return_move)
        {
        case 0:
          playerMove_frame = true;
          break;
        case -1:
          //   Logger::logMessage("ERROR [gameLoop] Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Up");
          //   errorMsg = "Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Up";
          //   return -1;
          break;

        default:
          if (interaction(return_move))
          {
            return 2;
          }
          break;
        }
        break;
      }

      case TerminalUtils::TerminalKeys::SHIFT_DOWN:
      case 'S':
      {
        return_move = movePlayer(playerLocation, currentMap, moveDirection::DOWN, true);
        switch (return_move)
        {
        case 0:
          playerMove_frame = true;
          break;
        case -1:
          // Logger::logMessage("ERROR [gameLoop] Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Shift Up");
          // errorMsg = "Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Shift Up";
          // return -1;
          break;

        default:
          if (return_move >= 100)
          {
            playerMove_frame = true;
            return_move -= 100;
            refreshScreen();
          }
          if (interaction(return_move))
          {
            return 2;
          }
          break;
        }
        break;
      }

      case TerminalUtils::TerminalKeys::LEFT:
      case 'a':
      {
        return_move = movePlayer(playerLocation, currentMap, moveDirection::LEFT);
        switch (return_move)
        {
        case 0:
          playerMove_frame = true;
          break;
        case -1:
          //   Logger::logMessage("ERROR [gameLoop] Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Up");
          //   errorMsg = "Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Up";
          //   return -1;
          break;

        default:
          if (interaction(return_move))
          {
            return 2;
          }
          break;
        }
        break;
      }

      case TerminalUtils::TerminalKeys::SHIFT_LEFT:
      case 'A':
      {
        return_move = movePlayer(playerLocation, currentMap, moveDirection::LEFT, true);
        switch (return_move)
        {
        case 0:
          playerMove_frame = true;
          break;
        case -1:
          // Logger::logMessage("ERROR [gameLoop] Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Shift Up");
          // errorMsg = "Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Shift Up";
          // return -1;
          break;

        default:
          if (return_move >= 100)
          {
            playerMove_frame = true;
            return_move -= 100;
            refreshScreen();
          }
          if (interaction(return_move))
          {
            return 2;
          }
          break;
        }
        break;
      }

      case TerminalUtils::TerminalKeys::RIGHT:
      case 'd':
      {
        return_move = movePlayer(playerLocation, currentMap, moveDirection::RIGHT);
        switch (return_move)
        {
        case 0:
          playerMove_frame = true;
          break;
        case -1:
          //   Logger::logMessage("ERROR [gameLoop] Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Up");
          //   errorMsg = "Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Up";
          //   return -1;
          break;

        default:
          if (interaction(return_move))
          {
            return 2;
          }
          break;
        }
        break;
      }

      case TerminalUtils::TerminalKeys::SHIFT_RIGHT:
      case 'D':
      {
        return_move = movePlayer(playerLocation, currentMap, moveDirection::RIGHT, true);
        switch (return_move)
        {
        case 0:
          playerMove_frame = true;
          break;
        case -1:
          // Logger::logMessage("ERROR [gameLoop] Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Shift Right");
          // errorMsg = "Fail to move player (row, col): " + std::to_string(playerLocation.row) + ' ' + std::to_string(playerLocation.col) + " to: Shift Right";
          // return -1;
          break;

        default:
          if (return_move >= 100)
          {
            playerMove_frame = true;
            return_move -= 100;
            refreshScreen();
          }
          if (interaction(return_move))
          {
            return 2;
          }
          break;
        }
        break;
      }

      case 'o':
      case 'O':
        TerminalUtils::printText("OPEN SHOP", -1, -2, true);
        break;

      case 'r':
      case 'R':
        refreshScreen(true);
        break;

      case 't':
      case 'T':
        showTips = !showTips;
        refreshScreen(true);
        break;

      case TerminalUtils::TerminalKeys::ESC:
      case 'p':
      case 'P':
        if (askExitGame())
        {
          return 1;
          break;
        }
        refreshScreen(true);
        break;

      default:
        break;
      }

      refreshScreen();

    } while (true);

    return 0;
  }

  void Game::refreshScreen(bool force)
  {

    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastRefreshTime);
    auto elapsedArtTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastArtRefreshTime);

    if (elapsedTime >= refreshInterval || force)
    {
      lastRefreshTime = currentTime; // Update the last refresh time

      if (TerminalUtils::getTerminalSize() != originalTerminalSize)
      {
        originalTerminalSize = TerminalUtils::getTerminalSize();
        force = true;
      }

      // If printing whole frame fps ~25 so if player move print only player location line
      if (playerMove_frame)
      {
        int start_row = (originalTerminalSize.rows - currentMap.size()) / 2;
        int start_col = (originalTerminalSize.cols - TerminalUtils::calculateAdjustedLength(currentMap[0])) / 2;
        TerminalUtils::printText(" ", start_row + oldPlayerLocation.row, start_col + oldPlayerLocation.col + 1, false);
        TerminalUtils::printText("P", start_row + playerLocation.row, start_col + playerLocation.col + 1, false);
        oldPlayerLocation = playerLocation; // Update old player location
        LastCurrentMap = currentMap;        // Update last current map
        playerMove_frame = false;
      }
      else if (LastCurrentMap != currentMap || force)
      {
        system("clear");
        TerminalUtils::printFrame(currentMap);
        LastCurrentMap = currentMap;

        TerminalUtils::printText("Coins: " + std::to_string(game_data.player_last_state.coins), 1, -1, false);

        if (showTips)
        {
          for (int i = 0; i < tips.size(); i++)
          {
            TerminalUtils::printText(tips[i], originalTerminalSize.rows - tips.size() + 1 + i, -3, false);
          }
        }
        else
        {
          TerminalUtils::printText("Press 'T' to show tips", originalTerminalSize.rows, -3, false);
        }
      }

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
        TerminalUtils::printText(sta, 1, -3, false);
      }
    }
  }

  std::string Game::replaceSubstring(const std::string &original, const std::string &toReplace, const std::string &replaceWith)
  {
    std::string result = original;
    size_t pos = 0;
    while ((pos = result.find(toReplace, pos)) != std::string::npos)
    {
      result.replace(pos, toReplace.length(), replaceWith);
      pos += replaceWith.length(); // Advance position past the replaced part
    }
    return result;
  }

  int Game::start()
  {
    // Load the game settings as menu may change the settings
    settingsManager.loadIniFile();
    Logger::logMessage("[INFO] [Game/start] Game run.");
    std::vector<std::string> introLines;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    int key, count = 0;
    if (!mapDetail.intro.empty())
    {
      for (auto &text : mapDetail.intro)
      {
        std::string modifiedText = replaceSubstring(text, "\\name/", settingsManager.Settings.playerName); // Replace "\name\" with "ABC"

        std::vector<std::string> introLines;
        std::string::size_type pos = 0;
        std::string::size_type prev = 0;
        while ((pos = modifiedText.find("\\n", prev)) != std::string::npos)
        {
          introLines.push_back(modifiedText.substr(prev, pos - prev));
          prev = pos + 2;
        }
        introLines.push_back(modifiedText.substr(prev));

        TerminalUtils::printFrame(introLines, 3, 1);
        int key = 0, count = 0;
        do
        {
          key = TerminalUtils::readKey();
          std::this_thread::sleep_for(std::chrono::milliseconds(50));
          ++count;
        } while (key == 0 && count < 45);
        count = 0;
        introLines.clear();
      }
    }
    std::vector<std::string> temp;
    for (int i = 1; i < 5; i++)
    {
      temp.push_back(tips[i].substr(1, tips[i].size() - 2));
      if (i == 2)
      {
        temp.push_back(" ");
        temp.push_back(" ");
      }
    }
    temp.push_back("");
    temp.push_back("");
    temp.push_back("Press anything to start the game...");
    TerminalUtils::printFrame(temp, 3);

    int c;
    do
    {
      c = TerminalUtils::readKey();
    } while (c == 0);

    refreshScreen();
    LastCurrentMap = currentMap;

    int return_key = gameLoop();

    switch (return_key)
    {
    case -1:
      Logger::logMessage("[ERROR] [Game/start] Game loop ended with error [" + errorMsg + "].");
      break;
    case 0:
      Logger::logMessage("[INFO] [Game/start] Game loop ended.");
      break;
    case 1:
      Logger::logMessage("[INFO] [Game/start] Game loop ended with user exit using ESC.");
      return 1;
      break;
    case 2:
      // Open next level
      Logger::logMessage("[INFO] [Game/start] Game loop ended with open next level.");
      return 2;
      break;

    default:
      break;
    }

    // double playerValue[3] = {500.0, 100.0, 25.0}; // 500=playerHP, 100=playerStrength, 25=playerResistence(e.g. emeny attack with 400 damage, player will take 400-25=375 damage)
    // std::vector<std::string> inventory = {"health_potion"};
    // std::vector<std::string> armorlist = {"rusty_iron_sword", "helmet"};
    // Game exit
    return 0;
  }
} // namespace MainGame
