#ifndef BATTLE_H
#define BATTLE_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <random>

#include "game.h"
#include "commonUtils.h"
#include "fileUtils.h"
#include "gameData.h"
#include "settings.h"
// 27.4
struct enemydata
{
  std::string enemyName;
  double enemyHp;
  double enemyAtk;
  double enemyDef;
  double enemyMp;

  std::string error;

  std::vector<int> enemySkill1;
  std::vector<int> enemySkill2;
  std::vector<int> enemySkill3;
  std::vector<int> enemySkill4;
  std::vector<int> enemyAbility;
  std::vector<int> rewardRange;

  // return enemy state
  std::vector<double> getEnemyState() const
  {
    return {enemyHp, enemyAtk, enemyDef, enemyMp};
  }
};

struct playerdata
{
  double hp;
  double atk;
  double def;
  double mp;

  std::string error;

  std::vector<int> skill1;
  std::vector<int> skill2;
  std::vector<int> skill3;
  std::vector<int> skill4;
  std::vector<int> inventory;

  // return player state
  std::vector<double> getPlayerState() const
  {
    return {hp, atk, def, mp};
  }
};

// 27.4
std::vector<int> stringToIntVector(const std::string &str, const char &saperator); // turn "1.2.3.4.5.6" to {1,2,3,4,5,6} by stringToIntVector("1.2.3.4.5.6", '.')
std::vector<std::string> stringToStringVector(const std::string &str); // turn "1.2.3.4.5.6" to {"1","2","3","4","5","6"} by stringToStringVector("1.2.3.4.5.6")
std::vector<std::string> split(const std::string &s); // split string to string vector
enemydata getEnemyData(const std::string &enemyCode); // read enemyData.txt and return it's stats, getEnemyData("13")
void cast(enemydata &stats, playerdata &stats1, std::vector<int> skill, std::string target); // cast skills, change the stats of player and enemy
bool ensure(enemydata current, std::vector<int> skill);  // ensure emeny have enough mana/wont kill themselves
bool ensurePlayer(playerdata player, std::vector<int> skill);  // ensure player have enough mana/wont kill himeself
int chooseSKill(double prob1, double prob2, double prob3, double prob4, double prob5);  //random choose skill and return skill code
std::vector<std::vector<int>> classify(std::vector<int> skill1, std::vector<int> skill2, std::vector<int> skill3);  //classify the skills (basic attack/Ultimate/sacrifice)
int enemyAi(enemydata &current, playerdata &player, std::string enemycode); //enemy choose and cast skills based on probability and it's health andplayer's health
void insert(std::vector<std::string> screen, int skillCode, playerdata player);  //insert the player choosing skill bar data
void changeValue(playerdata player, enemydata enemy, std::vector<std::string> screen, int round); //change the values on fight UI (e.g. player hp, enemy atk)
void announce(std::string msg, bool start, std::vector<std::string> screen);  // announce Round and enemy casted skill
void insertCharacter(std::string enemyCode, std::vector<std::string> screen, bool hide, int a, int b);  //insert Characters from enemy.txt to the fightUI 
void displaySkill(std::string skillcode, bool caster, int effect, std::vector<std::string> screen, int a, int b);  // player skill animation
int openInventory(playerdata &player, enemydata enemy);  // open inventory and let player use potion during the battle
bool remindWindow(std::string enemycode);  //give the player the enemydata he's going to face and ensure player wants to battle

int fight(playerdata &player, std::string enemycode, std::vector<std::string> screen);  //the main fight function, return 0 if player lose/leave game, return the rewards based on the range in enemyData.txt

#endif
