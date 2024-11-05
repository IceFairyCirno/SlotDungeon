// game.h
#ifndef GAME_H
#define GAME_H

#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

#include "gameData.h"
#include "settings.h"
#include "fileUtils.h"
#include "commonUtils.h"

struct location
{
  int row = 0;
  int col = 0;
  int byteCol = 0;
};

struct moveDirection
{
  static const bool UP[2];
  static const bool RIGHT[2];
  static const bool LEFT[2];
  static const bool DOWN[2];
};

struct collisionItems
{
  static const int wall;
  static const int slotMachine;
  static const int enemy;
  static const int shop;
  static const int door;
};

extern const std::unordered_map<std::string, int> char_list;

// Helper function to check if a character is an emoji
bool isEmojiAtPosition(const std::string &line, int byteIndex);

/**
 * @brief Moves the player in the game world on a 2D grid.
 *
 * This function updates the player's location based on the specified direction
 * and the length of the move (single or double step). It checks for boundaries,
 * empty spaces, and collisions, including special handling for emoji collisions,
 * and updates the game display accordingly. The function incorporates checks for
 * leftward movements and vertical movements to detect emojis and prevents moving
 * into those spaces, treating them as solid objects.
 *
 * @param playerLocation A reference to a 'location' struct representing the
 *        current x and y coordinates of the player on the grid. This structure
 *        is updated to the new location if the move is successful.
 * @param display_items A reference to a 2D vector of strings representing the game world,
 *        where each string in the vector represents a row in the game world. Each character
 *        in a string can be a space (' '), an emoji, or other symbols representing different
 *        game elements. The function modifies this vector to reflect the player's new position
 *        by clearing the previous position and marking the new position with 'P'.
 * @param direction An array of two booleans indicating the move direction:
 *        [true, false] for LEFT, [false, true] for RIGHT, [false, false] for UP,
 *        and [true, true] for DOWN. This controls how the function calculates the
 *        new position by setting row and column offsets.
 * @param len_2 A boolean indicating if the move should be a double step.
 *        If true, the player attempts to move two steps in the chosen direction;
 *        otherwise, a single step is attempted. If an obstacle or boundary is encountered
 *        during the first step of a double move, the move terminates at the first step.
 *
 * @return An integer code indicating the result of the move:
 *         0 if the move was successful and no collision occurred,
 *        -1 if the move was out of bounds (this function currently returns 0 in such cases for simplicity),
 *         or the collision code corresponding to the object encountered.
 *         If a move results in a collision, the collision code can be positive or negative,
 *         depending on the object type. An additional 100 may be added to the collision code
 *         to indicate both a successful move and a subsequent collision during a double step.
 *
 * The function uses a helper function 'isEmojiAtPosition' to determine if an emoji is present
 * at a given position in the display_items grid, using a regex pattern to match emojis within
 * a specified Unicode range. It also employs 'TerminalUtils::getByteIndexFromVisualIndex' to
 * handle variable-width characters properly by converting a visual column index to the corresponding
 * byte index in the string.
 */
int movePlayer(location &playerLocation, std::vector<std::string> &display_items, const bool direction[2], bool len_2 = false, bool forceRepalce = false);

// Helper function to get collision type
// @return collision code
int checkCollision(char item);

namespace MainGame
{
  class Game
  {
  private:
    std::unordered_map<std::string, int> enemyList{
        {"11", 1},
        {"12", 1},
        {"13", 1},
        {"14", 1},
        {"15", 1},
        {"16", 1},
        {"17", 1},
        {"18", 1},
        {"21", 2},
        {"22", 2},
        {"23", 2},
        {"24", 2},
        {"25", 2},
        {"26", 2},
        {"31", 3},
        {"32", 3},
    };

    std::string map_name;
    GameData::GameStats &game_data;
    GameSettings::SettingsManager settingsManager;
    bool DEBUG;
    bool SHOW_FPS;
    int FPS;
    double fpsToUs;
    std::vector<double> frameTimes;
    const size_t maxFrameTimes = 10;
    std::chrono::microseconds refreshInterval;
    std::chrono::microseconds refreshArtInterval;
    std::chrono::steady_clock::time_point lastRefreshTime, lastArtRefreshTime;

    TerminalUtils::TerminalSize originalTerminalSize;

    // true if player move frame
    // false if player didnt move
    bool playerMove_frame;

    location oldPlayerLocation;

    bool initializationError = false; // Flag to indicate initialization error

    std::vector<std::string> currentMap;
    std::vector<std::string> LastCurrentMap = {};
    std::vector<std::vector<std::string>> mapTxtSet, mapColorSet;
    FileUtils::_map_details mapDetail;

    location playerLocation;             // x, y
    std::vector<location> enemyLocation; // x, y

    // tips
    bool showTips = false;
    std::vector<std::string> tips{
        std::string(23, '#'),
        "#  W     Up           #",
        "# ASD  <Down>         #",
        "# R   - Reload screen #",
        "# ESC - Exit          #",
        std::string(23, '#'),
    };

    std::vector<std::string> warningDoorLock{
        std::string(53, ' '),
        ' ' + std::string(51, '#') + ' ',
        " #                                                 # ",
        " #          The door is fill by " + AnsiColors::colorFormat("darkness", Ansi.fg.red) + "           # ",
        " #         Kill " + AnsiColors::colorFormat("ALL", Ansi.fg.yellow) + " monster to go through          # ",
        " #                                                 # ",
        ' ' + std::string(51, '#') + ' ',
        std::string(53, ' '),
    };

    std::string replaceSubstring(const std::string &original, const std::string &toReplace, const std::string &replaceWith);

  public:
    std::string errorMsg;
    Game(const std::string &mapName,
         GameSettings::SettingsManager &settingsManager,
         GameData::GameStats &game_data,
         const bool &DEBUG);
    bool isError() const;
    int loadMap();
    int battle(double &hp,
               double strength,
               double resistance);

    // A function to ask the player if they want to exit the game
    bool askExitGame();

    // A function to handle player interaction
    bool interaction(int return_move);

    // The main game loop
    int gameLoop();

    // A function to refresh the screen
    void refreshScreen(bool force = false);

    // init the game like showing intro, etc
    int start();
  };
}

#endif // GAME_H
