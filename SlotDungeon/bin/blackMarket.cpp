#include <stdio.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <vector>

#include "gameData.h"
#include "commonUtils.h"
#include "ansi.h"

// detail for Product
struct Product
{
  std::string name;
  double ATK;
  double DEF;
  double HP;
  int MP;
  int price;
};

Product products[4] = {
    {"Heal potion", 0, 0, 200, 0, 500},
    {"ATK potion", 150, 0, 0, 0, 750},
    {"DEF potion", 0, 50, 0, 0, 500},
    {"MP potion", 0, 0, 0, 100, 1000},
};

// function for printing Product
void displayProduct(int emptyCol, int emptyRow, std::string productData, int selectedProduct, GameData::GameStats &data)
{ 
  int diff;
  for (int i = 0; i < emptyRow; i++)
    std::cout << '\n';
  std::cout << std::string(emptyCol, ' ') << productData << std::endl;
  for (int i = 0; i < 4; ++i)
  {
    std::cout << std::string(emptyCol, ' ');
    if (i == selectedProduct)
    {
      // highlight selected products (white backgroud)
      ansi_font_reverse();
      ansi_font_flash_fast();
      ansi_fg_white();
    }
    else
    {
      // reprint
      switch (i)
      {
      case 0:
        ansi_fg_green();
        break;
      case 1:
        ansi_fg_red();
        break;
      case 2:
        ansi_fg_yellow();
        break;
      case 3:
        ansi_fg_blue();
        break;
      }
    }
    diff = products[i].name.size() - 3;
    // alignment
    std::cout << products[i].name << std::setw(20 - diff);
    std::cout << products[i].ATK << std::setw(20);
    std::cout << products[i].DEF << std::setw(19);
    std::cout << products[i].HP << std::setw(20);
    std::cout << products[i].MP << std::setw(23);
    std::cout << products[i].price;
    ansi_reset();
    std::cout << std::endl;
  }
  // show the coins and numbers of potion
  std::cout << "Your coins: ";
  ansi_bg_yellow();
  ansi_fg_magenta();
  std::cout << data.player_last_state.coins << std::endl;
  ansi_reset();
  ansi_fg_green();
  std::cout << "HP potion: " << data.player_last_state.Inventory[0] << std::endl;
  ansi_fg_red();
  std::cout << "ATK potion: " << data.player_last_state.Inventory[1] << std::endl;
  ansi_fg_yellow();
  std::cout << "DEF potion: " << data.player_last_state.Inventory[2] << std::endl;
  ansi_fg_blue();
  std::cout << "MP potion: " << data.player_last_state.Inventory[3] << std::endl;
  ansi_reset();
  std::cout << "[ENTER]: BUY" << std::endl
            << "[Q]: QUIT" << std::endl;
  return;
}

// Call this function when you press ENTER
void buyProduct(GameData::GameStats &data, int selectedProduct)
{ 
  if (data.player_last_state.coins < products[selectedProduct].price)
  {
    std::cout << "NOT ENOUGH COINS!" << std::endl;
  }
  else
  {
    std::cout << "DEAL!" << std::endl;
    data.player_last_state.coins -= products[selectedProduct].price;
    data.player_last_state.Inventory[selectedProduct]+=1;
    // Update player's inventory and coins
  }
  ansi_reset();
  return;
}

void blackmarket(GameData::GameStats &data)
{
  // keyboard input
  int input = 0;
  bool quit = false;
  int selectedProduct = 0;
  int diff;
  system("clear");
  auto termSize = TerminalUtils::getTerminalSize();
  std::string productData = "Products            ATK                 DEF                 HP                  MP                  Price";
  int emptyCol = termSize.cols / 2 - productData.size() / 2;
  int emptyRow = termSize.rows / 2 - 3;
  // print empty rows

  // while loop for choosing products
  displayProduct(emptyCol, emptyRow, productData, selectedProduct, data);
  while (quit == false)
  {
    input = TerminalUtils::readKey();
    if (input == 0)
      continue;
    switch (input)
    {
    case ('w'):
    case ('W'):
    case (1000):
      selectedProduct = (selectedProduct == 0) ? 3 : selectedProduct - 1;
      system("clear");
      // Re-print the product list
      displayProduct(emptyCol, emptyRow, productData, selectedProduct, data);
      break;

    case ('s'):
    case ('S'):
    case (1001):
      selectedProduct = (selectedProduct == 3) ? 0 : selectedProduct + 1;
      system("clear");
      // Re-print the product list
      displayProduct(emptyCol, emptyRow, productData, selectedProduct, data);
      break;

    // If ENTER is pressed, buy the product.
    case (10):
      std::cout << std::string(termSize.cols / 2 - 4, ' ');
      ansi_font_flash();
      ansi_font_bold();
      ansi_fg_rgb(255, 204, 255);
      buyProduct(data, selectedProduct);
      break;
    // quit
    case ('q'):
    case ('Q'):
    case (27):
      quit = true;
      system("clear");
      break;
    }
  }
  return;
}
