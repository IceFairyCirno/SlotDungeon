#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <math.h>

#include <sstream>
#include <unistd.h>

#include "ansi.h"
#include "commonUtils.h"
#include "fileUtils.h"
#include "settings.h"
#include "slotMachine.h"


void createSlots(std::array<std::string, 30> &randomChars, char target)
{

  struct Letter
  {
    std::string r1;
    std::string r2;
    std::string r3;
    std::string r4;
    std::string r5;
    std::string r6;
  };

  Letter l[5];
  // Letter l0;    //null
  // Letter l1;    //common
  // Letter l2;    //rare
  // Letter l3;    //epic
  // Letter l4;    //legendary

  if (target == '%')
  {

    // R"( )" is used to avoid taking character behind the backslash as an
    // esacape sequence

    l[0].r1 = "      ";
    l[0].r2 = " __ __";
    l[0].r3 = R"( |\ ||)";
    l[0].r4 = R"( ||\||)";
    l[0].r5 = R"( || \|)";
    l[0].r6 = "      ";

    l[1].r1 = "  ___ ";
    l[1].r2 = R"( // \\)";
    l[1].r3 = R"( ||   )";
    l[1].r4 = R"( ||  _)";
    l[1].r5 = R"( \\_//)";
    l[1].r6 = "      ";

    l[3].r1 = " ____ ";
    l[3].r2 = R"( || \\)";
    l[3].r3 = " ||_//";
    l[3].r4 = R"( ||\\ )";
    l[3].r5 = R"( || \\)";
    l[3].r6 = "      ";

    l[2].r1 = "  ____";
    l[2].r2 = " ||   ";
    l[2].r3 = " ||___";
    l[2].r4 = " ||   ";
    l[2].r5 = " ||___";
    l[2].r6 = "      ";

    l[4].r1 = " __   ";
    l[4].r2 = " ||   ";
    l[4].r3 = " ||   ";
    l[4].r4 = " ||___";
    l[4].r5 = " |___|";
    l[4].r6 = "      ";

    int j = 0;

    // int i start from 12 to make slot 3 spin two letters behind from slot 2
    // for (int i=12; i<30; i+=6)
    for (int i = 12; i < 30; i += 6)
    {
      randomChars[i] = l[j].r1;
      randomChars[i + 1] = l[j].r2;
      randomChars[i + 2] = l[j].r3;
      randomChars[i + 3] = l[j].r4;
      randomChars[i + 4] = l[j].r5;
      randomChars[i + 5] = l[j].r6;
      j++;
    }

    for (int i = 0; i < 12; i += 6)
    {

      randomChars[i] = l[j].r1;
      randomChars[i + 1] = l[j].r2;
      randomChars[i + 2] = l[j].r3;
      randomChars[i + 3] = l[j].r4;
      randomChars[i + 4] = l[j].r5;
      randomChars[i + 5] = l[j].r6;
      j++;
    }
  }

  else if (target == '$' || target == '@')
  {

    l[0].r1 = "     ";
    l[0].r2 = "__ __";
    l[0].r3 = R"(|\ ||)";
    l[0].r4 = R"(||\||)";
    l[0].r5 = R"(|| \|)";
    l[0].r6 = "     ";

    l[1].r1 = " ___ ";
    l[1].r2 = R"(// \\)";
    l[1].r3 = R"(||   )";
    l[1].r4 = R"(||  _)";
    l[1].r5 = R"(\\_//)";
    l[1].r6 = "     ";

    l[3].r1 = "____ ";
    l[3].r2 = R"(|| \\)";
    l[3].r3 = "||_//";
    l[3].r4 = R"(||\\ )";
    l[3].r5 = R"(|| \\)";
    l[3].r6 = "     ";

    l[2].r1 = " ____";
    l[2].r2 = "||   ";
    l[2].r3 = "||___";
    l[2].r4 = "||   ";
    l[2].r5 = "||___";
    l[2].r6 = "     ";

    l[4].r1 = "__   ";
    l[4].r2 = "||   ";
    l[4].r3 = "||   ";
    l[4].r4 = "||___";
    l[4].r5 = "|___|";
    l[4].r6 = "     ";

    if (target == '$')
    {
      int j = 0;

      for (int i = 0; i < 30; i += 6)
      {

        randomChars[i] = l[j].r1;
        randomChars[i + 1] = l[j].r2;
        randomChars[i + 2] = l[j].r3;
        randomChars[i + 3] = l[j].r4;
        randomChars[i + 4] = l[j].r5;
        randomChars[i + 5] = l[j].r6;
        j++;
      }
    }

    else if (target == '@')
    {
      int j = 0;

      // int i start with 6 to make slot 1 spin one letter (each letter with
      // height of 6) behind from slot 2
      // for (int i=6; i<30; i+=6)

      for (int i = 6; i < 30; i += 6)
      {

        randomChars[i] = l[j].r1;
        randomChars[i + 1] = l[j].r2;
        randomChars[i + 2] = l[j].r3;
        randomChars[i + 3] = l[j].r4;
        randomChars[i + 4] = l[j].r5;
        randomChars[i + 5] = l[j].r6;
        j++;
      }

      // for (int i=0; i<12; i+=6)
      for (int i = 0; i < 6; i += 6)
      {

        randomChars[i] = l[j].r1;
        randomChars[i + 1] = l[j].r2;
        randomChars[i + 2] = l[j].r3;
        randomChars[i + 3] = l[j].r4;
        randomChars[i + 4] = l[j].r5;
        randomChars[i + 5] = l[j].r6;
        j++;
      }
    }
  }
}


void spinSlot(std::array<std::string, 30> randomChars, int row, int col,
              char target, int shifting, char result, bool &endHead, bool &endBottom, bool time)
{

  int changeNRows = 7;

  // endSlot is used to find when to stop each slot
  std::string endHeadSlot = "";
  std::string endBottomSlot = "";

  if (result == 'N')
  {
    if (target == '%')
    {
      endHeadSlot = " __ __";
      endBottomSlot = "      ";
    }

    else if ((target == '$') || (target == '@'))
    {
      endHeadSlot = "__ __";
      endBottomSlot = "     ";
    }
  }

  else if (result == 'C')
  {

    if (target == '%')
    {
      endHeadSlot = "  ___ ";
      endBottomSlot = "      ";
    }

    else if ((target == '$') || (target == '@'))
    {
      endHeadSlot = " ___ ";
      endBottomSlot = "     ";
    }
  }

  else if (result == 'R')
  {

    if (target == '%')
    {
      endHeadSlot = " ____ ";
      endBottomSlot = R"( || \\)";
    }

    else if ((target == '$') || (target == '@'))
    {
      endHeadSlot = "____ ";
      endBottomSlot = R"(|| \\)";
    }
  }

  else if (result == 'E')
  {

    if (target == '%')
    {
      endHeadSlot = "  ____";
      endBottomSlot = " ||___";
    }

    else if ((target == '$') || (target == '@'))
    {
      endHeadSlot = " ____";
      endBottomSlot = "||___";
    }
  }

  else if (result == 'L')
  {

    if (target == '%')
    {
      endHeadSlot = " __   ";
      endBottomSlot = " |___|";
    }

    else if ((target == '$') || (target == '@'))
    {
      endHeadSlot = "__   ";
      endBottomSlot = "|___|";
    }
  }

  else
  {
    // Logger::logMessage("Wrong rarity");
  }

  if (endHeadSlot == "" || endBottomSlot == "")
  {
    // Logger::logMessage("No endSlot");
  }

  for (int i = 0; i < changeNRows; ++i)
  {
    int idx;
    idx = (shifting + i) % 30;

    if (time)
    {
      if ((i == 0) && (randomChars[idx] == endHeadSlot))
      {
        endHead = true;
      }

      else if ((i == changeNRows - 1) && (randomChars[idx] == endBottomSlot))
      {
        endBottom = true;
      }
    }

    bool stopPrintFrame = (endHead && endBottom);

    if (!stopPrintFrame)
    {
      TerminalUtils::printText(randomChars[idx], row + i, col, false);
    }
  }
}


char randomPull()
{

  srand(time(NULL));

  struct Treasures
  {
    char rarity;
    float probability;
  };

  Treasures treasure[5];

  // NULL
  treasure[0].rarity = 'N';
  treasure[0].probability = 0.2;

  // Common
  treasure[1].rarity = 'C';
  treasure[1].probability = 0.3;

  // Rare
  treasure[2].rarity = 'R';
  treasure[2].probability = 0.4;

  // Epic
  treasure[3].rarity = 'E';
  treasure[3].probability = 0.5;

  // Legendary
  treasure[4].rarity = 'L';
  treasure[4].probability = 0.6;

  for (int i = 0; i < 5; i++)
  {
    char currentRarity = treasure[i].rarity;

    float currentProb = treasure[i].probability;

    float randomNumber = rand() % 100;

    // if the randomly generated number is larger than the probability
    // then you may get the higher rarity prize

    if (randomNumber > (currentProb * 100))
    {
      continue;
    }

    else
    {
      return currentRarity;
    }
  }

  return 'L';
}


char runSlot()
{
  // system("clear");

  std::vector<std::string> map =
      FileUtils::readLinesFromFile("content/art/slotMachine.txt");
  int numRows = map.size();
  int numCols = map[0].size();

  auto T_size = TerminalUtils::getTerminalSize();

  // Find location of printing text
  int col1, col2, col3;
  int startRow = (T_size.rows - map.size()) / 2 + 10;

  int startCol = (T_size.cols - TerminalUtils::calculateAdjustedLength(map[0])) / 2;
  col1 = startCol + 15;
  col2 = startCol + 47;
  col3 = startCol + 80;

  TerminalUtils::printFrame(
      map); // print the outer frame of the slotMachine first

  auto duration_step = std::chrono::milliseconds(200);
  std::this_thread::sleep_for(duration_step);

  int shifting = 0;

  // these arrays store all the spinning letters in each slot with different
  // orders
  std::array<std::string, 30> randomChars1;
  std::array<std::string, 30> randomChars2;
  std::array<std::string, 30> randomChars3;

  createSlots(randomChars1, '$');
  createSlots(randomChars2, '%');
  createSlots(randomChars3, '@');

  // pull for the random prize
  char result = randomPull();

  bool endHead1 = false, endBottom1 = false;

  bool endHead2 = false, endBottom2 = false;

  bool endHead3 = false, endBottom3 = false;

  auto startTime = std::chrono::steady_clock::now();

  bool stopPrintFrame1, stopPrintFrame2, stopPrintFrame3, allSlotStop;

  while (true)
  {

    auto currentTime = std::chrono::steady_clock::now();

    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(
                           currentTime - startTime)
                           .count();

    auto duration_step = std::chrono::milliseconds(0);

    // this base decide when to end the spinning

    stopPrintFrame1 = (endHead1 && endBottom1);

    stopPrintFrame2 = (endHead2 && endBottom2);

    stopPrintFrame3 = (endHead3 && endBottom3);

    allSlotStop = (stopPrintFrame1 && stopPrintFrame2 && stopPrintFrame3);

    if (allSlotStop)
    {
      break;
    }

    bool stopTime = false;

    if (elapsedTime >= 9)
    {
      stopTime = true;
    }

    // spinning slot 1
    if (!stopPrintFrame1)
    {
      spinSlot(randomChars1, startRow, col1, '$', shifting, result, endHead1, endBottom1, stopTime);
    }

    // spinning slot 2
    if (!stopPrintFrame2)
    {
      spinSlot(randomChars2, startRow, col2, '%', shifting, result,
               endHead2, endBottom2, stopTime);
    }

    // spinning slot 3
    if (!stopPrintFrame3)
    {
      spinSlot(randomChars3, startRow, col3, '@', shifting, result,
               endHead3, endBottom3, stopTime);
    }

    // perform the slowdown animation
    if (elapsedTime >= 10)
    {
      auto duration_step = std::chrono::milliseconds(200);
      std::this_thread::sleep_for(duration_step);
    }

    else if (elapsedTime >= 8)
    {
      auto duration_step = std::chrono::milliseconds(100);
      std::this_thread::sleep_for(duration_step);
    }

    else if (elapsedTime >= 6)
    {
      auto duration_step = std::chrono::milliseconds(50);
      std::this_thread::sleep_for(duration_step);
    }

    // shifting allows the shifting
    shifting++;
  }

  return result;
}


void openTreasure(char result, char symbol)
{

  std::vector<std::string> treasure =
      FileUtils::readLinesFromFile("content/art/treasure.txt");

  TerminalUtils::printFrame(treasure);

  int numRows = treasure.size();
  int numCols = treasure[0].size();

  int startRow, endRow, col, changeNRow;

  for (int row = 0; row < numRows; ++row)
  {
    col = treasure[row].find('%'); // text

    if ((col != -1))
    {
      startRow = row;
      break;
    }
    else
    {
      // Logger::logMessage("cannot find the location of all slots");
    }
  }

  std::string prize, rarity, buff;
  int numSpaces;

  switch (result)
  {
  case 'N':
    prize = "DemonLord";
    rarity = "???";
    if (symbol == 'A')
      buff = "50";
    if (symbol == 'D')
      buff = "10";
    if (symbol == 'H')
      buff = "100";
    numSpaces = 34;
    break;

  case 'C':
    prize = "Champion of Destruction";
    rarity = "Common";
    if (symbol == 'A')
      buff = "50";
    if (symbol == 'D')
      buff = "10";
    if (symbol == 'H')
      buff = "100";
    numSpaces = 26;
    break;

  case 'R':
    prize = "Titan of Warfare";
    rarity = "Rare";
    if (symbol == 'A')
      buff = "100";
    if (symbol == 'D')
      buff = "20";
    if (symbol == 'H')
      buff = "200";
    numSpaces = 30;
    break;

  case 'E':
    prize = "Supreme Overlord";
    rarity = "Epic";
    if (symbol == 'A')
      buff = "200";
    if (symbol == 'D')
      buff = "40";
    if (symbol == 'H')
      buff = "400";
    numSpaces = 30;
    break;

  case 'L':
    prize = "God of Wars";
    rarity = "Legendary";
    if (symbol == 'A')
      buff = "300";
    if (symbol == 'D')
      buff = "60";
    if (symbol == 'H')
      buff = "600";
    numSpaces = 30;
    break;

  default:
    break;
  }

  std::string status;
  if (symbol == 'A')
    status = "ATK";
  if (symbol == 'D')
    status = "DEF";
  if (symbol == 'H')
    status = "HP";

  std::string message1, message2;
  std::string sign;

  if (result == 'N')
  {
    message1 =
        std::string(31, ' ') +
        "Ha!Ha! you have been cursed by the"; //                             ";
    sign = "-";
  }

  else
  {
    message1 = "                         Congratulations, you have been "
               "blessed by the"; //                          ";
    sign = "+";
  }

  message2 = std::string(numSpaces, ' ') + prize + " < " + rarity +
             " > , " + status + " " + sign +
             buff; // + std::string(numSpaces, ' ');

  changeNRow = 2;

  std::string message[2] = {message1, message2};

  endRow = startRow + changeNRow;

  auto duration_step = std::chrono::milliseconds(1000);
  std::this_thread::sleep_for(duration_step);

  int n = 0;

  for (int i = startRow; i < endRow; i++)
  {

    int changeNCols = message[n].length();

    treasure[i].erase(col, changeNCols);

    treasure[i].insert(col, message[n]);

    TerminalUtils::printFrame(treasure);

    auto duration_step = std::chrono::milliseconds(1000);
    std::this_thread::sleep_for(duration_step);

    n++;
  }
}


bool opening(int &coins)
{
  system("clear");
  std::vector<std::string> opening = FileUtils::readLinesFromFile("content/art/opening.txt");

  TerminalUtils::printFrame(opening);

  int numRows = opening.size();
  int numCols = opening[0].size();

  auto T_size = TerminalUtils::getTerminalSize();

  // Find location of printing text
  int startRow1, startRow2, col1, col2;
  int startRow = (T_size.rows - opening.size()) / 2;
  startRow1 = startRow + 2;
  startRow2 = startRow + 17;
  int startCol = (T_size.cols - TerminalUtils::calculateAdjustedLength(opening[0])) / 2;
  col1 = startCol + 34;
  col2 = startCol + 26;

  std::vector<std::string> messages = {
      "Welcome, Adventurer, to the place of Kumara, the God of Games.",
      "Here is a slot machine that you may receive blesses or curses.",
      "Are you ready to test your luck?",
      "Coins: " + std::to_string(coins) + " / 2000",
      "( Press p to spin once )",
      "( Sorry, you dont have enough coins )"};

  messages = TerminalUtils::alignTextToMaxLength(messages);

  std::string message6 = AnsiColors::colorFormat(messages[5], Ansi.fg.red, Ansi.font.flash_fast);
  messages.pop_back();
  std::string message5 = AnsiColors::colorFormat(messages[4], Ansi.fg.yellow, Ansi.font.flash_fast);
  messages.pop_back();

  // init sleep
  auto duration_step = std::chrono::milliseconds(1000);
  std::this_thread::sleep_for(duration_step);

  int key, count = 0;

  // loop sleep for allowing press key to skip line
  auto duration_step_msg = std::chrono::milliseconds(10);

  int n = 0;

  for (int i = 0; i < messages.size(); i++)
  {
    key = 0;
    count = 0;
    TerminalUtils::printText(messages[i], startRow1 + i * 2, col1, false);
    do
    {
      key = TerminalUtils::readKey();
      std::this_thread::sleep_for(duration_step_msg);
      ++count;
      if (key == TerminalUtils::TerminalKeys::ESC)
      {
        return false;
      }
    } while (count < 30 && key == 0);
  }

  TerminalUtils::printText(message5, startRow + 12, col1, false);

  std::vector<std::string> prizes =
      {
          "Blesses (on either one stat): ",
          "Champion of Destruction < Common > , ATK/DEF/HP +50/+10/+100",
          "Titan of Warfare < Rare >, ATK/DEF/HP +100/+20/+200",
          "Supreme Overlord < Epic >, ATK/DEF/HP +200/+40/+400",
          "God of Wars < Legendary >, ATK/DEF/HP +300/+60/+600",
          "",
          "Curse:",
          "DemonLord < ??? >, ATK/DEF/HP -50/10/100",
      };

  for (int i = 0; i < prizes.size(); i++)
  {
    key = 0;
    count = 0;
    TerminalUtils::printText(prizes[i], startRow2 + i, col2, false);
    do
    {
      key = TerminalUtils::readKey();
      std::this_thread::sleep_for(duration_step_msg);
      ++count;
    } while (count < 30 && key == 0);
  }

  bool hvEnoughCoins = false;
  key = 0;
  do
  {
    do
    {
      key = TerminalUtils::readKey();
    } while (key == 0);

    if (key == 'p' || key == 'P')
    {
      if (coins < 2000)
      {
        TerminalUtils::printText(message6, startRow1 + 10, col1, false);
        key = 0;
      }
      else
      {
        coins -= 2000;
        hvEnoughCoins = true;
      }
    }
  } while (key != 'p' && key != 'P' && key != TerminalUtils::TerminalKeys::ESC);
  Logger::logMessage("Slot machine opened and return: " + std::to_string(hvEnoughCoins) + " coins: " + std::to_string(coins));
  return hvEnoughCoins;
}


void runSlotMachine(int &stat, int &coins, char symbol)
{
  
  if (opening(coins))
  {
    auto duration_step1 = std::chrono::milliseconds(1000);
    std::this_thread::sleep_for(duration_step1);

    char result = runSlot();

    auto duration_step2 = std::chrono::milliseconds(1000);
    std::this_thread::sleep_for(duration_step2);

    openTreasure(result, symbol);

    switch (result)
    {
    case 'N':
      if (symbol == 'A')
        stat = std::max(stat - 50, 50);
      if (symbol == 'D')
        stat = std::max(stat - 10, 10);
      if (symbol == 'H')
        stat = std::max(stat - 100, 300);
      break;

    case 'C':
      if (symbol == 'A')
        stat += 50;
      if (symbol == 'D')
        stat += 10;
      if (symbol == 'H')
        stat += 100;
      break;

    case 'R':
      if (symbol == 'A')
        stat += 100;
      if (symbol == 'D')
        stat += 20;
      if (symbol == 'H')
        stat += 200;
      break;

    case 'E':
      if (symbol == 'A')
        stat += 200;
      if (symbol == 'D')
        stat += 40;
      if (symbol == 'H')
        stat += 400;
      break;

    case 'L':
      if (symbol == 'A')
        stat += 300;
      if (symbol == 'D')
        stat += 60;
      if (symbol == 'H')
        stat += 600;
      break;

    default:
      break;
    }

    auto duration_step3 = std::chrono::milliseconds(1000);
    std::this_thread::sleep_for(duration_step3);

    int key;
    do
    {
      key = TerminalUtils::readKey();
    } while (key == 0);
  }
}

void chooseStat(int &ATK, int &DEF, int &HP, int &coins)
{
  srand(time(NULL));
  int randomNumber = rand() % 3;
  if (randomNumber == 0)
  {
    runSlotMachine(ATK, coins, 'A');
  }
  else if (randomNumber == 1)
  {
    runSlotMachine(DEF, coins, 'D');
  }
  else if (randomNumber == 2)
  {
    runSlotMachine(HP, coins, 'H');
  }
}

/*You can try to run the slotMachine with
g++ -g slotMachine.cpp terminalUtils.cpp fileUtils.cpp ansi.cpp
colorTextUtils.cpp settings.cpp logger.cpp -o slotMachine

and remove the comment below
*/
/*
int main() {

  Logger logger;
  logger.deleteOldLogs();
  Logger::logMessage("[DEBUG] [main] Log Initialized");

  //required parameters atk and coins
  int atk = 1000;
  int coins = 10000;


  runSlotMachine(atk, coins);                                   //only need this line for game.cpp



  //the following lines are just for checking if the atk and coins have been changed
  system("clear"); std::string statusChange = "ATK: " + std::to_string(atk) + ", Coins: " + std::to_string(coins);
  TerminalUtils::printText(statusChange);
}*/
