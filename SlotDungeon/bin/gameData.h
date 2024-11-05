#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <string>
#include <vector>

namespace GameData
{
  struct playerLastState
  {
    // Player initial stats
    int HP = 450;
    int ATK = 150;
    int DEF = 20;
    int coins = 2001;
    // int coins = 100000;

    // Health potion, Attack potion, Defense potion, MP potion
    std::vector<int> Inventory{
        1,  // Health potion
        1,  // Attack potion
        1,  // Defense potion
        1}; // MP potion
  };

  struct GameStats
  {
    bool error = false;

    // only change current max level when player pass that map
    // set it to the map the player passed, not the next one
    int currentMaxLevel = 0;
    // int currentMaxLevel = 4; // to skip all the way to the last level

    // player last state
    // save player state when player earn increase stats
    playerLastState player_last_state;
  };

  // Saves the data to a file called "gameStats.dat"
  // Takes a GameStats struct as input
  // Returns void
  bool saveGameStats(const GameStats &stats);

  // Load game stats from file
  // Returns a GameStats struct
  GameStats loadGameStats();
}

#endif // GAME_DATA_H