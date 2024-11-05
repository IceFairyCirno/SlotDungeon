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
#include "battle.h"

std::string enemyData_path = "content/Data/enemyData.txt";
std::string enemyArt_path = "content/art/enemy.txt";
std::string fightUI_path = "content/art/fightUI.txt";
std::string inventory_path = "content/art/inventory.txt"; // 28.4
std::string remindWindow_path = "content/art/remindWindow.txt";

std::vector<int> stringToIntVector(const std::string &str, const char &saperator)
{
  std::vector<int> intVector;
  size_t pos = 0;
  size_t foundPos;
  std::string temp;
  while ((foundPos = str.find(saperator, pos)) != std::string::npos)
  {
    temp = str.substr(pos, foundPos - pos);
    intVector.push_back(std::stoi(temp));
    pos = foundPos + 1;
  }
  temp = str.substr(pos);
  intVector.push_back(std::stoi(temp));
  return intVector;
}

std::vector<std::string> stringToStringVector(const std::string &str)
{
  std::vector<std::string> stringVector;
  size_t pos = 0;
  size_t foundPos;
  std::string temp;
  while ((foundPos = str.find("          ", pos)) != std::string::npos)
  {
    temp = str.substr(pos, foundPos - pos);
    stringVector.push_back(temp);
    pos = foundPos + 1;
  }
  temp = str.substr(pos);
  stringVector.push_back(temp);
  return stringVector;
}

std::vector<std::string> split(const std::string &s)
{
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (tokenStream >> token)
  {
    tokens.push_back(token);
  }
  return tokens;
}

enemydata getEnemyData(const std::string &enemyCode)
{
  std::ifstream file(enemyData_path);
  if (!file.is_open())
  {
    enemydata enemy;
    enemy.error = "Error opening file: " + enemyData_path;
    return enemy; // Return error if file can't be opened
  }

  std::string line;
  getline(file, line); // Skip header line

  while (getline(file, line))
  {
    auto data = split(line);
    if (data.size() != 11 || data[0] != enemyCode)
      continue;

    try
    {
      enemydata enemy;
      enemy.enemyName = data[1];
      enemy.enemyHp = std::stod(data[2]);
      enemy.enemyAtk = std::stod(data[3]);
      enemy.enemyDef = std::stod(data[4]);
      enemy.enemySkill1 = stringToIntVector(data[5], '_');
      enemy.enemySkill2 = stringToIntVector(data[6], '_');
      enemy.enemySkill3 = stringToIntVector(data[7], '_');
      enemy.enemySkill4 = stringToIntVector(data[8], '_');
      enemy.enemyAbility = stringToIntVector(data[9], '_');
      enemy.rewardRange = stringToIntVector(data[10], '-');
      file.close();
      return enemy;
    }
    catch (std::exception &e)
    {
      enemydata enemy;
      enemy.error = "Parsing error on line: " + line + "; " + e.what();
      file.close();
      return enemy;
    }
  }

  file.close();
  enemydata enemy;
  enemy.error = "Enemy code not found";
  return enemy; // Return error if code not found
}

void cast(enemydata &stats, playerdata &stats1, std::vector<int> skill, std::string target)
{ // ok
  if (skill.size() == 4)
  {
    if (target == "enemy")
    {
      stats.enemyHp += skill[0];
      stats1.hp -= skill[1] - stats1.def;
      stats.enemyDef += skill[2];
      stats.enemyMp += skill[3];
    }
    else
    {
      stats1.hp += skill[0];
      stats.enemyHp -= skill[1] - stats.enemyDef;
      stats1.def += skill[2];
      stats1.mp += skill[3];
    }
  }
  else
  {
    if (skill[1] == 0)
    {
      stats.enemyHp += skill[2];
      if (skill[3] != 0)
      {
        stats1.hp = stats1.hp - skill[3] + stats1.def;
      }
      stats.enemyDef += skill[4];
      stats.enemyMp += skill[5];
    }
    else
    {
      stats1.hp += skill[2];
      if (skill[3] != 0)
      {
        stats.enemyHp = stats.enemyHp - skill[3] + stats.enemyDef;
      }
      stats1.def += skill[4];
      stats1.mp += skill[5];
    }
  }
  stats.enemyHp = std::max(stats.enemyHp, 0.0);
  stats.enemyAtk = std::max(stats.enemyAtk, 0.0);
  stats.enemyDef = std::max(stats.enemyDef, 0.0);
  stats.enemyMp = std::max(stats.enemyMp, 0.0);
  stats1.hp = std::max(stats1.hp, 0.0);
  stats1.atk = std::max(stats1.atk, 0.0);
  stats1.def = std::max(stats1.def, 0.0);
  stats1.mp = std::max(stats1.mp, 0.0);
}

bool ensure(enemydata current, std::vector<int> skill)
{
  if (current.enemyHp + skill[0] > 0 && current.enemyDef + skill[2] >= 0 && current.enemyMp + skill[3] >= 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

bool ensurePlayer(playerdata player, std::vector<int> skill)
{
  if (player.hp + skill[0] > 0 && player.def + skill[2] >= 0 && player.mp + skill[3] >= 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int chooseSKill(double prob1, double prob2, double prob3, double prob4, double prob5)
{
  std::vector<int> prob;
  // prob1=A, prob2=sk1, prob3=sk2, prob4=sk3, prob5=ultimate
  for (int i = 0; i < prob1 * 10; i++)
  {
    prob.push_back(0);
  }
  for (int i = 0; i < prob2 * 10; i++)
  {
    prob.push_back(1);
  }
  for (int i = 0; i < prob3 * 10; i++)
  {
    prob.push_back(2);
  }
  for (int i = 0; i < prob4 * 10; i++)
  {
    prob.push_back(3);
  }
  for (int i = 0; i < prob5 * 10; i++)
  {
    prob.push_back(4);
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, prob.size() - 1);
  int index = dis(gen);
  return prob[index];
}

std::vector<std::vector<int>> classify(std::vector<int> skill1, std::vector<int> skill2, std::vector<int> skill3)
{
  std::vector<std::vector<int>> set = {skill1, skill2, skill3};
  std::vector<int> heal = skill1, damage = skill1, sacrifice = {0, 0, 0, 0}, general = {0, 0, 0, 0}, empty = {0, 0, 0, 0};
  for (int i = 0; i < 3; i++)
  {
    if (set[i][1] > damage[1])
    {
      damage = set[i];
    } // identify damage skill
    if (set[i][0] > heal[0])
    {
      heal = set[i];
    } // identify heal skill
    if (set[i][0] < 0 || set[i][2] < 0)
    {
      sacrifice = set[i];
    }
  }
  if (sacrifice == empty)
  {
    for (int j = 0; j < 3; j++)
    {
      if (set[j] != damage && set[j] != heal)
      {
        general = set[j];
      }
    }
  }

  return {heal, damage, sacrifice, general};
}

int enemyAi(enemydata &current, playerdata &player, std::string enemycode)
{

  std::vector<std::vector<int>> skillSet = classify(current.enemySkill1, current.enemySkill2, current.enemySkill3);

  std::vector<int> heal = skillSet[0];
  std::vector<int> damage = skillSet[1];
  std::vector<int> sacrifice = skillSet[2];
  std::vector<int> general = skillSet[3], empty = {0, 0, 0, 0};

  int castedSkill;

  skillSet.push_back(current.enemySkill4);
  enemydata original = getEnemyData(enemycode);
  original.enemyMp = 0;
  int size = skillSet.size();
  for (int i = 0; i < size; i++)
  {
    if (skillSet[i] == empty)
    {
      skillSet.erase(skillSet.begin() + i);
    }
  }
  skillSet.insert(skillSet.begin(), {0, int(current.enemyAtk), 0, 10});

  // skillSet[0]=A, skillSet[1]=heal, skillSet[2]=damage, skillSet[3]=sacrifice/general, skillSet[4]=ultimate,
  // prob1=A, prob2=heal, prob3=damage, prob4=sacrifice/general, prob5=ultimate

  if (skillSet[0][1] >= player.hp || skillSet[1][1] >= player.hp || skillSet[2][1] >= player.hp || skillSet[3][1] >= player.hp)
  { // kill player
    // std::cout<<"kill player"<<std::endl;
    if (skillSet[1][1] >= player.hp && ensure(current, skillSet[1]))
    {
      cast(current, player, skillSet[1], "enemy");
      castedSkill = 1;
    }
    else if (skillSet[2][1] >= player.hp && ensure(current, skillSet[2]))
    {
      cast(current, player, skillSet[2], "enemy");
      castedSkill = 2;
    }
    else if (skillSet[3][1] >= player.hp && ensure(current, skillSet[3]))
    {
      cast(current, player, skillSet[3], "enemy");
      castedSkill = 3;
    }
    else
    {
      cast(current, player, skillSet[0], "enemy");
      castedSkill = 0;
    }
  }
  else if (ensure(current, skillSet[4]))
  { // ultimate
    // std::cout<<"ultimate"<<std::endl;
    cast(current, player, skillSet[4], "enemy");
    castedSkill = 4;
  }
  else if (current.enemyHp <= original.enemyHp * 0.2 && ensure(current, skillSet[1]))
  { // heal
    // std::cout<<"heal"<<std::endl;
    cast(current, player, skillSet[1], "enemy");
    castedSkill = 1;
  }
  else if (current.enemyHp <= original.enemyHp * 0.4)
  {
    int index = chooseSKill(0.2, 0.5, 0.3, 0.2, 0.1);
    // std::cout<<"health0.4"<<std::endl;
    while (!ensure(current, skillSet[index]))
    {
      index = chooseSKill(0.2, 0.5, 0.3, 0.2, 0.1);
    }
    // std::cout<<"health0.4done"<<std::endl;
    cast(current, player, skillSet[index], "enemy");
    castedSkill = index;
  }
  else if (current.enemyHp <= original.enemyHp * 0.8)
  {
    int index = chooseSKill(0.5, 0.3, 0.6, 0.4, 0.2);
    // std::cout<<"health0.8"<<std::endl;
    while (!ensure(current, skillSet[index]))
    {
      index = chooseSKill(0.5, 0.3, 0.6, 0.4, 0.2);
    }
    // std::cout<<"health0.8done"<<std::endl;
    cast(current, player, skillSet[index], "enemy");
    castedSkill = index;
  }
  else if (current.enemyHp <= original.enemyHp)
  {
    int index = chooseSKill(0.2, 0.4, 0.6, 0.1, 0.3);
    // std::cout<<"health0.8"<<std::endl;
    while (!ensure(current, skillSet[index]))
    {
      index = chooseSKill(0.2, 0.4, 0.6, 0.1, 0.3);
    }
    // std::cout<<"health0.8done"<<std::endl;
    cast(current, player, skillSet[index], "enemy");
    castedSkill = index;
  }
  else if (current.enemyHp <= original.enemyHp * 1.5)
  {
    if (sacrifice != empty)
    {
      // std::cout<<"health1.5 sacrifice"<<std::endl;
      int index = chooseSKill(0.6, 0.1, 0.5, 0.8, 0.5);
      while (!ensure(current, skillSet[index]))
      {
        index = chooseSKill(0.6, 0.1, 0.5, 0.8, 0.5);
      }
      // std::cout<<"health1.5 sacrifice done"<<std::endl;
      cast(current, player, skillSet[index], "enemy");
      castedSkill = index;
    }
    else
    {
      // std::cout<<"health1.5"<<std::endl;
      int index = chooseSKill(0.5, 0.3, 0.5, 0.3, 0.4);
      while (!ensure(current, skillSet[index]))
      {
        index = chooseSKill(0.5, 0.3, 0.5, 0.3, 0.4);
      }
      // std::cout<<"health1.5 done"<<std::endl;
      cast(current, player, skillSet[index], "enemy");
      castedSkill = index;
    }
  }
  else
  {
    if (sacrifice != empty)
    {
      // std::cout<<"default sacrifice"<<std::endl;
      int index = chooseSKill(0.7, 0.1, 0.6, 0.9, 0.6);
      while (!ensure(current, skillSet[index]))
      {
        index = chooseSKill(0.7, 0.1, 0.6, 0.9, 0.6);
      }
      // std::cout<<"default sacrifice done"<<std::endl;
      cast(current, player, skillSet[index], "enemy");
      castedSkill = index;
    }
    else
    {
      // std::cout<<"default"<<std::endl;
      int index = chooseSKill(0.6, 0.2, 0.7, 0.5, 0.3);
      while (!ensure(current, skillSet[index]))
      {
        index = chooseSKill(0.6, 0.2, 0.7, 0.5, 0.3);
      }
      // std::cout<<"default done"<<std::endl;
      cast(current, player, skillSet[index], "enemy");
      castedSkill = index;
    }
  }
  return castedSkill;
}

void insert(std::vector<std::string> screen, int skillCode, playerdata player)
{
  std::vector<std::string> skillName = {"1. BC Attack", "2. Recovery!", "3. Sacrifice", "4. AD Attack", "5. Ultimate!"};
  std::string value;

  switch (skillCode)
  {
  case 1:
    value = "{ Hp+0|Damage: " + std::to_string(int(player.atk)) + "|Def+0|Mp+10 }";
    break;
  case 2:
    value = "{ Hp+" + std::to_string(player.skill1[0]) + "|Damage: " + std::to_string(player.skill1[1]) + "|Def" + std::to_string(player.skill1[2]) + "|Mp+" + std::to_string(player.skill1[3]) + " }";
    break;
  case 3:
    value = "{ Hp" + std::to_string(player.skill2[0]) + "|Damage: " + std::to_string(player.skill2[1]) + "|Def+" + std::to_string(player.skill2[2]) + "|Mp+" + std::to_string(player.skill2[3]) + " }";
    break;
  case 4:
    value = "{ Hp+" + std::to_string(player.skill3[0]) + "|Damage: " + std::to_string(player.skill3[1]) + "|Def+" + std::to_string(player.skill3[2]) + "|Mp" + std::to_string(player.skill3[3]) + " }";
    break;
  case 5:
    value = "{ Hp+" + std::to_string(player.skill4[0]) + "|Damage: " + std::to_string(player.skill4[1]) + "|Def+" + std::to_string(player.skill4[2]) + "|Mp" + std::to_string(player.skill4[3]) + " }";
    break;

  default:
    break;
  }
  int no_space = (42 - value.length()) / 2;
  value = std::string(no_space, ' ') + value + std::string(42 - no_space - value.length(), ' ');

  auto T_size = TerminalUtils::getTerminalSize();
  int totalrow = T_size.rows;
  int totalcol = T_size.cols;
  int row = screen.size();
  int col = TerminalUtils::calculateAdjustedLength(screen[0]);

  int startrow = std::max((totalrow - row) / 2, 1);
  int startcol = std::max((totalcol - col) / 2, 1);
  std::string spaces(skillName[skillCode - 1].length() + 4, ' ');
  TerminalUtils::printText(spaces, startrow + 23, -2, false);
  TerminalUtils::printText(skillName[skillCode - 1], startrow + 23, -2, false); // name
  TerminalUtils::printText(value, startrow + 24, -2, false);                    // data
}

void changeValue(playerdata player, enemydata enemy, std::vector<std::string> screen, int round)
{
  auto T_size = TerminalUtils::getTerminalSize();
  int totalrow = T_size.rows;
  int totalcol = T_size.cols;
  int row = screen.size();
  int col = TerminalUtils::calculateAdjustedLength(screen[0]);

  int startrow = std::max((totalrow - row) / 2, 1);
  int startcol = std::max((totalcol - col) / 2, 1);
  int endcol = std::max((totalcol - col) / 2, 1) + col;

  // Print txt and color with range (e.g. hp < 200 yellow, hp < 100 red)
  // {"HP:  ", {{200, Ansi.fg.byellow}, {100, Ansi.fg.bred}}}
  // Skip {"ATK: ", {}},
  std::vector<std::pair<std::string, std::pair<std::pair<int, std::string>, std::pair<int, std::string>>>> output_state{
      {"HP:  ", {{200, Ansi.fg.yellow}, {100, Ansi.fg.red}}},
      {"ATK: ", {}},
      {"DEF: ", {}},
      {"MP:  ", {{10000, Ansi.fg.bcyan}, {1, Ansi.fg.white}}},
  };
  auto playerState = player.getPlayerState();
  std::string printTxt;
  for (int repeat = 0; repeat < 2; repeat++)
  {
    for (int i = 0; i < playerState.size(); i++)
    {
      if (output_state[i].second.first.first != 0)
      {
        if (playerState[i] < output_state[i].second.second.first)
        {
          printTxt = AnsiColors::colorFormat(std::to_string(int(playerState[i])), output_state[i].second.second.second);
        }
        else if (playerState[i] < output_state[i].second.first.first)
        {
          printTxt = AnsiColors::colorFormat(std::to_string(int(playerState[i])), output_state[i].second.first.second);
        }
        else
        {
          printTxt = std::to_string(int(playerState[i]));
        }
      }
      else
      {
        printTxt = std::to_string(int(playerState[i]));
      }
      TerminalUtils::printText(output_state[i].first + printTxt + "   ", startrow + i + 1, startcol + (repeat == 0 ? 3 : 81), false);
    }
    TerminalUtils::printText("Round " + std::to_string(int(round)), startrow + 1, -2, false); // Round
    playerState = enemy.getEnemyState();
  }
}

void announce(std::string msg, bool start, std::vector<std::string> screen)
{
  auto T_size = TerminalUtils::getTerminalSize();
  int totalrow = T_size.rows;
  int totalcol = T_size.cols;
  int row = screen.size();
  int col = TerminalUtils::calculateAdjustedLength(screen[0]);

  int startrow = std::max((totalrow - row) / 2, 1);
  int startcol = std::max((totalcol - col) / 2, 1);
  std::string spaces(msg.length(), ' ');

  if (start == 1)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  TerminalUtils::printText(msg, startrow + 10, -2, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  TerminalUtils::printText(spaces, startrow + 10, -2, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
}

void insertCharacter(std::string enemyCode, std::vector<std::string> screen, bool hide, int a, int b)
{
  std::ifstream file(enemyArt_path);
  std::string line;
  std::vector<std::string> art;
  bool start = false;
  while (std::getline(file, line))
  {
    if (line.find(enemyCode + "S") != std::string::npos)
    {
      start = true;
      continue;
    }
    else if (line.find(enemyCode + "E") != std::string::npos)
    {
      break;
    }
    else if (start)
    {
      art.push_back(line);
    }
  }
  file.close();

  auto T_size = TerminalUtils::getTerminalSize();
  int totalrow = T_size.rows;
  int totalcol = T_size.cols;
  int row = screen.size();
  int col = TerminalUtils::calculateAdjustedLength(screen[0]);
  int startrow = std::max((totalrow - row) / 2, 1);
  int startcol = std::max((totalcol - col) / 2, 1);

  if (!hide)
  {
    for (int i = art.size() - 1; i > -1; i--)
    {
      TerminalUtils::printText(art[i], startrow + a + i - (art.size() - 1), startcol + b, false);
    }
  }
  else
  {
    for (int i = art.size() - 1; i > -1; i--)
    {
      std::string space(art[i].length(), ' ');
      TerminalUtils::printText(space, startrow + a + i - (art.size() - 1), startcol + b, false);
    }
  }
}

void displaySkill(std::string skillcode, bool caster, int effect, std::vector<std::string> screen, int a, int b)
{
  // 1=move effect, 2=blink effect, 3=laser effect
  if (effect == 1)
  {
    for (int i = 0; i < 54; i++)
    {
      insertCharacter(skillcode, screen, 0, a, b + i);
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
      insertCharacter(skillcode, screen, 1, a, b + i);
    }
  }
  else if (effect == 2)
  {
    for (int i = 0; i < 10; i++)
    {
      insertCharacter(skillcode, screen, 0, a, b);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      insertCharacter(skillcode, screen, 1, a, b);
    }
  }
  else
  {
    std::ifstream file(enemyArt_path);
    std::string line;
    std::vector<std::string> art;
    bool start = false;
    while (std::getline(file, line))
    {
      if (line.find(skillcode + "S") != std::string::npos)
      {
        start = true;
        continue;
      }
      else if (line.find(skillcode + "E") != std::string::npos)
      {
        break;
      }
      else if (start)
      {
        art.push_back(line);
      }
    }
    file.close();

    auto T_size = TerminalUtils::getTerminalSize();
    int totalrow = T_size.rows;
    int totalcol = T_size.cols;
    int row = screen.size();
    int col = TerminalUtils::calculateAdjustedLength(screen[0]);
    int startrow = std::max((totalrow - row) / 2, 1);
    int startcol = std::max((totalcol - col) / 2, 1);

    for (int k = 0; k < art[0].length(); k++)
    {
      for (int j = art.size() - 1; j > -1; j--)
      {
        std::string point;
        point += art[j][k];
        TerminalUtils::printText(point, startrow + a + j - (art.size() - 1), startcol + k + b, false);
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    insertCharacter(skillcode, screen, 1, a, b);
  }
}

int openInventory(playerdata &player, enemydata enemy)
{
  std::vector<std::string> inventory = FileUtils::readLinesFromFile(inventory_path), set = {"HP", "AP", "DP", "MP"}, name = {"Heal Potion", "ATK  Potion", "Defs Potion", "Mana Potion"}, effect = {"HP+200 ", "ATK+150", "DEF+50 ", "MP+100 "};
  TerminalUtils::printFrame(inventory);

  std::vector<int> count = {0, 0, 0, 0};
  int code = 1;
  for (int i = 0; i < (player.inventory).size(); i++)
  {
    switch (player.inventory[i])
    {
    case 1:
      count[0] += 1;
      break;
    case 2:
      count[1] += 1;
      break;
    case 3:
      count[2] += 1;
      break;
    case 4:
      count[3] += 1;
      break;
    }
  }
  
  insertCharacter("HP", inventory, 0, 14, 45);
  TerminalUtils::printText(name[code - 1], 23, -2, 0);
  TerminalUtils::printText(effect[code - 1], 24, -2, 0);
  TerminalUtils::printText("You have " + std::to_string(count[code - 1]) + " Left", 25, -2, 0);
  
  int key;
  do
  {
    do
    {
      key = TerminalUtils::readKey();
    } while (key == 0);

    switch (key)
    {
    case TerminalUtils::TerminalKeys::LEFT:
    case 'a':
    case 'A':
      if (code == 1)
      {
        insertCharacter("HP", inventory, 1, 14, 45);
        code = 4;
        insertCharacter("MP", inventory, 0, 14, 45);
        TerminalUtils::printText(name[code - 1], 23, -2, 0);
        TerminalUtils::printText(effect[code - 1], 24, -2, 0);
        TerminalUtils::printText("You have " + std::to_string(count[code - 1]) + " Left", 25, -2, 0);
      }
      else
      {
        insertCharacter(set[code - 1], inventory, 1, 14, 45);
        code = code - 1;
        insertCharacter(set[code - 1], inventory, 0, 14, 45);
        TerminalUtils::printText(name[code - 1], 23, -2, 0);
        TerminalUtils::printText(effect[code - 1], 24, -2, 0);
        TerminalUtils::printText("You have " + std::to_string(count[code - 1]) + " Left", 25, -2, 0);
      }
      break;

    case TerminalUtils::TerminalKeys::RIGHT:
    case 'd':
    case 'D':
      if (code == 4)
      {
        insertCharacter("MP", inventory, 1, 14, 45);
        code = 1;
        insertCharacter("HP", inventory, 0, 14, 45);
        TerminalUtils::printText(name[code - 1], 23, -2, 0);
        TerminalUtils::printText(effect[code - 1], 24, -2, 0);
        TerminalUtils::printText("You have " + std::to_string(count[code - 1]) + " Left", 25, -2, 0);
      }
      else
      {
        insertCharacter(set[code - 1], inventory, 1, 14, 45);
        code = code + 1;
        insertCharacter(set[code - 1], inventory, 0, 14, 45);
        TerminalUtils::printText(name[code - 1], 23, -2, 0);
        TerminalUtils::printText(effect[code - 1], 24, -2, 0);
        TerminalUtils::printText("You have " + std::to_string(count[code - 1]) + " Left", 25, -2, 0);
      }
      break;
    }
  } while (key != TerminalUtils::TerminalKeys::ENTER && key != TerminalUtils::TerminalKeys::ESC);
  if (key == TerminalUtils::TerminalKeys::ESC || count[code - 1] == 0)
  {
    return 0;
  }
  auto it = std::find((player.inventory).begin(), (player.inventory).end(), code);
  if (it != (player.inventory).end())
  {
    (player.inventory).erase(it);
  }

  return code;
}

bool remindWindow(std::string enemycode)
{
  std::vector<std::string> remindWindow = FileUtils::readLinesFromFile(remindWindow_path);
  TerminalUtils::printFrame(remindWindow);
  enemydata enemy = getEnemyData(enemycode);

  auto T_size = TerminalUtils::getTerminalSize();
  int totalrow = T_size.rows;
  int totalcol = T_size.cols;
  int row = remindWindow.size();
  int col = TerminalUtils::calculateAdjustedLength(remindWindow[0]);
  int startrow = std::max((totalrow - row) / 2, 1);
  int startcol = std::max((totalcol - col) / 2, 1);

  std::string skill0 = "{Skill 0: HP 0|Damage: " + std::to_string(int(enemy.enemyAtk)) + "|Def 0|Mp+10}";
  std::string skill1 = "{Skill 1: HP " + std::to_string(enemy.enemySkill1[0]) + "|Damage: " + std::to_string(enemy.enemySkill1[1]) + "|Def " + std::to_string(enemy.enemySkill1[2]) + "|Mp " + std::to_string(enemy.enemySkill1[3]) + "}";
  std::string skill2 = "{Skill 2: HP " + std::to_string(enemy.enemySkill2[0]) + "|Damage: " + std::to_string(enemy.enemySkill2[1]) + "|Def " + std::to_string(enemy.enemySkill2[2]) + "|Mp " + std::to_string(enemy.enemySkill2[3]) + "}";
  std::string skill3 = "{Skill 3: HP " + std::to_string(enemy.enemySkill3[0]) + "|Damage: " + std::to_string(enemy.enemySkill3[1]) + "|Def " + std::to_string(enemy.enemySkill3[2]) + "|Mp " + std::to_string(enemy.enemySkill3[3]) + "}";
  std::string skill4 = "{Skill 4: HP " + std::to_string(enemy.enemySkill4[0]) + "|Damage: " + std::to_string(enemy.enemySkill4[1]) + "|Def " + std::to_string(enemy.enemySkill4[2]) + "|Mp " + std::to_string(enemy.enemySkill4[3]) + "}";

  TerminalUtils::printText(enemy.enemyName, startrow + 2, -2, false);
  TerminalUtils::printText(skill0, startrow + 3, -2, false);
  TerminalUtils::printText(skill1, startrow + 4, -2, false);
  TerminalUtils::printText(skill2, startrow + 5, -2, false);
  TerminalUtils::printText(skill3, startrow + 6, -2, false);
  TerminalUtils::printText(skill4, startrow + 7, -2, false);

  int key;
  do
  {
    key = TerminalUtils::readKey();
  } while (key == 0);
  switch (key)
  {
  case 'Y':
  case 'y':
    return true;
    break;
  case 'N':
  case 'n':
    return false;
    break;
  }
  return false;
}

int fight(playerdata &player, std::string enemycode, std::vector<std::string> screen)
{
  // initialization
  enemydata enemystats = getEnemyData(enemycode);
  if (!enemystats.error.empty())
  {
    Logger::logMessage("have error : " + enemystats.error);
  }
  enemystats.enemyMp = 0;
  int round = 1;
  TerminalUtils::printFrame(screen, 3);
  insert(screen, 1, player);
  insertCharacter("00", screen, 0, 19, 4);
  insertCharacter(enemycode, screen, 0, 19, 73);
  changeValue(player, enemystats, screen, round);

  // gameloop
  while (player.hp > 0 && enemystats.enemyHp > 0)
  {
    if (round % 2 != 0) // player turn
    {                   // cast enemy ability
      changeValue(player, enemystats, screen, round);
      announce("Round " + std::to_string(round), 1, screen);
      if (enemystats.enemyAbility[0] == 1 && round == 1) // one time only ability
      {
        cast(enemystats, player, enemystats.enemyAbility, "enemy");
        // Recalculate skill
        player.skill1 = {std::max(int(player.hp * 0.25), 100), int(player.atk * 0.8), -5, 10};
        player.skill2 = {-(std::max(int(player.hp * 0.5), 100)), int(player.atk * 1.7), 15, 30};
        player.skill3 = {int(player.hp * 0.1), int(player.atk * 1.5), 10, -20};
        player.skill4 = {int(player.hp * 0.15), int(player.atk * 2), 10, -100};
        announce("Enemy Ability Casted", 1, screen);
        changeValue(player, enemystats, screen, round);
      }
      else if (enemystats.enemyAbility[0] == 0) // cast per two round ability
      {
        cast(enemystats, player, enemystats.enemyAbility, "enemy");
        // Recalculate skill
        player.skill1 = {std::max(int(player.hp * 0.25), 100), int(player.atk * 0.8), -5, 10};
        player.skill2 = {-(std::max(int(player.hp * 0.5), 100)), int(player.atk * 1.7), 15, 30};
        player.skill3 = {int(player.hp * 0.1), int(player.atk * 1.5), 10, -20};
        player.skill4 = {int(player.hp * 0.15), int(player.atk * 2), 10, -100};
        announce("Enemy Ability Casted", 1, screen);
        changeValue(player, enemystats, screen, round);
      }

      int key, code = 1;

      do
      {
        do
        {
          key = TerminalUtils::readKey();
        } while (key == 0);

        switch (key) // player switching the skills
        {
        case TerminalUtils::TerminalKeys::LEFT:
        case 'a':
        case 'A':
          if (code == 1)
          {
            code = 5;
            insert(screen, code, player);
          }
          else
          {
            code = code - 1;
            insert(screen, code, player);
          }
          break;

        case TerminalUtils::TerminalKeys::RIGHT:
        case 'd':
        case 'D':
          if (code == 5)
          {
            code = 1;
            insert(screen, code, player);
          }
          else
          {
            code = code + 1;
            insert(screen, code, player);
          }
          break;
        case 'I':
        case 'i':

          int used = openInventory(player, enemystats);
          if (used == 1)
          {
            player.hp += 200;
          }
          if (used == 2)
          {
            player.atk += 150;
          }
          if (used == 3)
          {
            player.def += 50;
          }
          if (used == 4)
          {
            player.mp += 100;
          }

          // Recalculate skill
          player.skill1 = {std::max(int(player.hp * 0.25), 100), int(player.atk * 0.8), -5, 10};
          player.skill2 = {-(std::max(int(player.hp * 0.5), 100)), int(player.atk * 1.7), 15, 30};
          player.skill3 = {int(player.hp * 0.1), int(player.atk * 1.5), 10, -20};
          player.skill4 = {int(player.hp * 0.15), int(player.atk * 2), 10, -100};

          // player.inventory.erase(std::find(player.inventory.begin(), player.inventory.end(), used));

          TerminalUtils::printFrame(screen, 2);
          insert(screen, 1, player);
          insertCharacter("00", screen, 0, 19, 4);
          insertCharacter(enemycode, screen, 0, 19, 73);

          changeValue(player, enemystats, screen, round);
          break;
        }
      } while (key != TerminalUtils::TerminalKeys::ENTER && key != TerminalUtils::TerminalKeys::ESC);
      if (key == TerminalUtils::TerminalKeys::ESC)
      {
        return 0;
      }
      if (code == 1) // cast skills
      {
        displaySkill("S1", 1, 1, screen, 19, 19);
        cast(enemystats, player, {0, int(player.atk), 0, 10}, "player");
        changeValue(player, enemystats, screen, round);
        insertCharacter("00", screen, 0, 19, 4);
        insertCharacter(enemycode, screen, 0, 19, 73);
      }
      else if (code == 2 && ensurePlayer(player, player.skill1))
      {
        displaySkill("S2", 1, 2, screen, 19, 4);
        cast(enemystats, player, player.skill1, "player");
        changeValue(player, enemystats, screen, round);
        insertCharacter("00", screen, 0, 19, 4);
        insertCharacter(enemycode, screen, 0, 19, 73);
      }
      else if (code == 3 && ensurePlayer(player, player.skill2))
      {
        displaySkill("S3", 1, 3, screen, 19, 19);
        cast(enemystats, player, player.skill2, "player");
        changeValue(player, enemystats, screen, round);
        insertCharacter("00", screen, 0, 19, 4);
        insertCharacter(enemycode, screen, 0, 19, 73);
      }
      else if (code == 4 && ensurePlayer(player, player.skill3))
      {
        displaySkill("S4", 1, 1, screen, 19, 19);
        cast(enemystats, player, player.skill3, "player");
        changeValue(player, enemystats, screen, round);
        insertCharacter("00", screen, 0, 19, 4);
        insertCharacter(enemycode, screen, 0, 19, 73);
      }
      else if (code == 5 && ensurePlayer(player, player.skill4))
      {
        displaySkill("S5", 1, 3, screen, 19, 19);
        cast(enemystats, player, player.skill4, "player");
        changeValue(player, enemystats, screen, round);
        insertCharacter("00", screen, 0, 19, 4);
        insertCharacter(enemycode, screen, 0, 19, 73);
      }
      else
      {
        TerminalUtils::printText("You don't have enough mana to use this skill", -1, -2, false);
        TerminalUtils::printText(std::string(44, ' '), -1, -2, false);
      }
      round += 1;
    }
    else
    {
      changeValue(player, enemystats, screen, round);
      announce("Round " + std::to_string(round) + "   ", 1, screen);
      int casted = enemyAi(enemystats, player, enemycode);
      announce("Enemy Casted Skill " + std::to_string(casted), 1, screen);
      changeValue(player, enemystats, screen, round);
      round += 1;
    }
  }

  if (player.hp != 0)
  {
    // death animation
    insertCharacter(enemycode, screen, 1, 19, 73);
    for (int q = 0; q < 5; q++)
    {
      insertCharacter(enemycode, screen, 0, 19, 73);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      insertCharacter(enemycode, screen, 1, 19, 73);
    }
    announce("You win!", 1, screen);
    // return coins
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(enemystats.rewardRange[0], enemystats.rewardRange[1]);
    int reward = dis(gen);

    return reward;
  }
  else
  {
    insertCharacter("00", screen, 1, 19, 4);
    for (int q = 0; q < 5; q++)
    {
      insertCharacter("00", screen, 0, 19, 4);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      insertCharacter("00", screen, 1, 19, 4);
    }
    announce("You are dead", 1, screen);
    return 0;
  }
}

/*
int main()
{
  Logger logger;
  logger.deleteOldLogs();
  Logger::logMessage("[DEBUG] [main] Log Initialized");

  TerminalUtils::setupTerminal();
  Logger::logMessage("[DEBUG] [main] Terminal Initialized");

  // for testing only
  // enemydata data= getEnemyData("11");
  playerdata player;
  player.hp = 450;
  player.atk = 150;
  player.def = 20;
  player.mp = 0;

  player.skill1 = {int(player.hp * 0.25), int(player.atk * 0.8), -5, 10};
  player.skill2 = {-(std::max(int(player.hp * 0.5), 100)), int(player.atk * 1.7), 15, 30};
  player.skill3 = {int(player.hp * 0.1), int(player.atk * 1.4), 10, -20};
  player.skill4 = {int(player.hp * 0.15), int(player.atk * 2.5), 10, -100};
  player.inventory = {1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4};
  std::vector<std::string> screen = FileUtils::readLinesFromFile(fightUI_path);
  //random enemy choose
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(11, 18);
  int enemy = dis(gen);
  fight(player, std::to_string(enemy), screen);
  return 0;
}
*/
