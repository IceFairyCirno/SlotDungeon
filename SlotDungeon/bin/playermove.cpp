
// #include <iostream>
// #include <vector>
// #include <string>
// #ifdef _WIN32
// #include <conio.h>
// #else
// #include <termios.h>
// #include <unistd.h>
// #include <stdio.h>
// #endif

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <regex>

#include "commonUtils.h"
// #include "fileUtils.h"
#include "game.h"

const bool moveDirection::UP[2] = {false, false};
const bool moveDirection::RIGHT[2] = {false, true};
const bool moveDirection::LEFT[2] = {true, false};
const bool moveDirection::DOWN[2] = {true, true};

const int collisionItems::wall = 0;
const int collisionItems::slotMachine = 1;
const int collisionItems::enemy = 2;
const int collisionItems::shop = 3;
const int collisionItems::door = 4;

const std::unordered_map<std::string, int> char_list = {
    {"▉", collisionItems::wall},
    {"▇", collisionItems::wall},
    {"🎰", collisionItems::slotMachine},
    {"👾", collisionItems::enemy},
    {"🏪", collisionItems::shop},
    {"🔒", collisionItems::door},
};

// Helper function to get collision type
int checkCollision(location target, std::vector<std::string> &display_items)
{
  if (target.row < 0 || target.row >= display_items.size())
  {
    Logger::logMessage("[ERROR] [checkCollision] Row out of bounds");
    return -1;
  }
  int byteIndex = TerminalUtils::getByteIndexFromVisualIndex(display_items[target.row], target.col);
  if (byteIndex == -1 || byteIndex >= display_items[target.row].size())
  {
    Logger::logMessage("[ERROR] [checkCollision] Byte index out of bounds: " + std::to_string(byteIndex));
    return -1;
  }

  const std::string &row_content = display_items[target.row];
  std::string currentChar = row_content.substr(byteIndex, TerminalUtils::utf8_char_length(static_cast<unsigned char>(row_content[byteIndex])));

  switch (currentChar[0])
  {
  case '@':
  case 'X':
    return collisionItems::wall;
    break;
  case 'S':
    return collisionItems::slotMachine;
    break;
  case 'E':
    return collisionItems::enemy;
    break;
  default:
    // Check for multi-character symbols
    Logger::logMessage("[INFO] [checkCollision] Checking for multi-character symbols: " + currentChar);
    for (const auto &item : char_list)
    {
      if (byteIndex + item.first.length() <= row_content.length() &&
          row_content.substr(byteIndex, item.first.length()) == item.first)
      {
        Logger::logMessage("[INFO] [checkCollision] Collision detected: " + item.first);
        return item.second;
        break;
      }
    }
    Logger::logMessage("[ERROR] [checkCollision] No valid collision object found at position");
    if (currentChar[0] == ' ')
    {
      // return -1; // Empty space
      return 0;
    }
    return 0; // No collision
    break;
  }
}

int movePlayer(location &playerLocation, std::vector<std::string> &display_items, const bool direction[2], bool len_2, bool forceRepalce)
{
  int rowOffset = 0;
  int colOffset = 0;

  // Mapping direction to offsets
  if (direction[0] == direction[1])
    rowOffset = direction[0] ? 1 : -1; // DOWN or UP
  else
    colOffset = direction[1] ? 1 : -1; // RIGHT or LEFT

  location targetLocation = playerLocation;
  targetLocation.row += rowOffset;
  targetLocation.col += colOffset; // `col` is visual index, increment visually.

  // Get byte index for the target visual column index.
  int byteIndex = TerminalUtils::getByteIndexFromVisualIndex(display_items[targetLocation.row], targetLocation.col);

  // Check bounds
  if (targetLocation.row < 0 || targetLocation.row >= display_items.size() || byteIndex == -1 || byteIndex >= display_items[targetLocation.row].size())
  {
    return 0; // Out of bounds
  }

  // Check if the position is empty (' ' in this case represents an empty space visually)
  if (display_items[targetLocation.row][byteIndex] == ' ' || forceRepalce)
  {
    // Logger::logMessage("[DEBUG] [movePlayer] Moving player to target location: " + std::to_string(targetLocation.row) + ", " + std::to_string(targetLocation.col));
    // additional emoji check
    if ((colOffset == -1 || rowOffset != 0) && !forceRepalce)
    {
      int additional_check_Byte = TerminalUtils::getByteIndexFromVisualIndex(display_items[targetLocation.row], std::max(targetLocation.col - (colOffset == -1 ? 5 : 1), 0));
      // Check if the player is moving left and there is an emoji at the current position
      // Logger::logMessage("[DEBUG] [movePlayer] Checking for emoji at position: " + std::to_string(additional_check_Byte));
      if (isEmojiAtPosition(display_items[targetLocation.row], additional_check_Byte))
      {
        // Prevent the player from moving left if there is an emoji at the current position
        targetLocation.col -= 1;
        // return checkCollision(targetLocation, display_items);
        int collision = checkCollision(targetLocation, display_items);
        return collision;
      }
    }

    // Handling double step movement
    if (len_2)
    {
      location nextLocation = targetLocation;
      nextLocation.row += rowOffset;
      nextLocation.col += colOffset; // Move visually again.

      // Get byte index for the next visual column index.
      // Improvement available: check only if col changes
      int nextByteIndex = TerminalUtils::getByteIndexFromVisualIndex(display_items[nextLocation.row], nextLocation.col);
      // int nextPreByteIndex = TerminalUtils::getByteIndexFromVisualIndex(display_items[nextLocation.row], nextLocation.col-1);

      // if ((nextByteIndex == nextPreByteIndex+1 && display_items[nextLocation.row][nextByteIndex] == ' ') || (nextPreByteIndex != nextPreByteIndex+1 && display_items[nextLocation.row][nextByteIndex - !direction[1] ? 1 : 0] == ' '))
      if (display_items[nextLocation.row][nextByteIndex] == ' ' && !(nextLocation.row < 0 || nextLocation.row >= display_items.size() || nextByteIndex == -1 || nextByteIndex >= display_items[nextLocation.row].size()))
      {
        if (rowOffset != 0)
        {
          int additional_check_Byte = TerminalUtils::getByteIndexFromVisualIndex(display_items[nextLocation.row], nextLocation.col - 1);

          // Check if the player is moving left and there is an emoji at the current position
          if (isEmojiAtPosition(display_items[nextLocation.row], additional_check_Byte))
          {
            // Prevent the player from moving left if there is an emoji at the current position
            nextLocation.col -= 1;
            // Move one step only
            display_items[playerLocation.row][TerminalUtils::getByteIndexFromVisualIndex(display_items[playerLocation.row], playerLocation.col)] = ' '; // Clear old position
            display_items[targetLocation.row][byteIndex] = 'P';                                                                                         // Set new position
            targetLocation.byteCol = byteIndex;
            playerLocation = targetLocation; // Update player's location

            return checkCollision(nextLocation, display_items) + 100;
          }
        }

        // Move player 2 steps
        byteIndex = nextByteIndex;
        targetLocation = nextLocation;
        // Logger::logMessage("[DEBUG] [movePlayer] Moving player 2 steps targetLocation.col: " + std::to_string(targetLocation.col) + ", byteIndex: " + std::to_string(byteIndex));
      }
      else
      {
        // Move player 1 step and return
        // Logger::logMessage("[DEBUG] [movePlayer] Moving player not 2 but 1 steps targetLocation.col: " + std::to_string(targetLocation.col) + ", byteIndex: " + std::to_string(byteIndex));
        display_items[playerLocation.row][TerminalUtils::getByteIndexFromVisualIndex(display_items[playerLocation.row], playerLocation.col)] = ' '; // Clear old position
        display_items[targetLocation.row][byteIndex] = 'P';                                                                                         // Set new position
        targetLocation.byteCol = byteIndex;
        playerLocation = targetLocation; // Update player's location
        int collision = checkCollision(nextLocation, display_items);

        return collision + (collision == -1 ? 0 : 100);
      }
    }

    // Move player
    int currentByteIndex = TerminalUtils::getByteIndexFromVisualIndex(display_items[playerLocation.row], playerLocation.col);
    display_items[playerLocation.row][currentByteIndex] = ' '; // Clear old position
    display_items[targetLocation.row][byteIndex] = 'P';        // Set new position
    targetLocation.byteCol = byteIndex;
    playerLocation = targetLocation; // Update player's location
    return 0;                        // No collision, successful move
  }
  else
  {
    // Logger::logMessage("[DEBUG] [movePlayer] Collision detected at target location");
    if (colOffset == -1)
    {
      targetLocation.col -= 1;
    }
    return checkCollision(targetLocation, display_items); // Collision at single step
  }
}

bool isEmojiAtPosition(const std::string &line, int byteIndex)
{
  if (byteIndex < 0 || byteIndex >= line.size())
  {
    return false; // Out of bounds, no emoji.
  }

  std::unordered_map<std::string, int> exclusionList = {
      {"▉", collisionItems::wall},
      {"▇", collisionItems::wall}};

  // Check if the character at byteIndex is one of the excluded characters
  for (const auto &pair : exclusionList)
  {
    std::string key = pair.first;
    if (line.compare(byteIndex, key.length(), key) == 0)
    {
      return false; // This character is not an emoji (it's a special character we want to exclude)
    }
  }

  std::regex emojiPattern(u8"[\U0001F600-\U0001F64F]"); // Emoji range
  std::smatch match;
  // Extract the substring from byteIndex to the end of the line
  std::string substring = line.substr(byteIndex);
  if (std::regex_search(substring, match, emojiPattern, std::regex_constants::match_continuous))
  {
    return match.size() > 0;
  }
  return false;
}
