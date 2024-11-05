#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "commonUtils.h"
#include "gameData.h"

const std::string gameData_filename = "bin/gameStats.dat";

namespace GameData
{

  std::string xorCipher(const std::string &input, char key)
  {
    std::string output = input;
    for (size_t i = 0; i < input.size(); i++)
    {
      output[i] = input[i] ^ key;
    }
    return output;
  }

  bool saveGameStats(const GameStats &stats)
  {
    std::ostringstream oss;
    oss << stats.currentMaxLevel << "\n";
    std::string plaintext = oss.str();

    char key = 'X'; // Choose a key for the XOR cipher
    std::string ciphertext = xorCipher(plaintext, key);

    std::ofstream file(gameData_filename);
    if (file.is_open())
    {
      file << ciphertext;
      file.close();
      Logger::logMessage("[DEBUG] Saved game stats.");
    }
    else
    {
      Logger::logMessage("[ERROR] Failed to save game stats.");
      return false;
    }
    return true;
  }

  GameStats loadGameStats()
  {
    GameStats stats;
    std::string ciphertext;

    std::ifstream file(gameData_filename);
    if (!file.good())
    {
      // The file does not exist, so create it.
      Logger::logMessage("[DEBUG] [loadGameStats] Creating " + gameData_filename + ".");
      // Create a new file with default values
      if (!saveGameStats(stats))
      {
        Logger::logMessage("[ERROR] [loadGameStats] Failed to create " + gameData_filename + ".");
        stats.error = true;
        return stats;
      }
      file.close();                 // Close the file to reset its state
      file.open(gameData_filename); // Reopen the file
    }

    if (file.is_open())
    {
      std::getline(file, ciphertext, '\0');
      file.close();
    }
    else
    {
      Logger::logMessage("[ERROR] Failed to open " + gameData_filename + " for reading.");
      stats.error = true;
      return stats;
    }

    char key = 'X'; // Use the same key as before
    std::string decryptedtext = xorCipher(ciphertext, key);

    std::istringstream iss(decryptedtext);
    iss >> stats.currentMaxLevel;
    Logger::logMessage("[DEBUG] Loaded game stats.");
    return stats;
  }

}

// Reference:
//