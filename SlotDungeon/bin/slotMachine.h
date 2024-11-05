#ifndef SLOT_MACHINE_H
#define SLOT_MACHINE_H

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

/*
Function createSlots, is used to store the different symbols ('N', 'C', 'R', 'E', 'L')
that will be displaced in each slot

Input:
1) randomChars: an array, with size 30 that can store string elements
    reference variable

2) target: a character, that represents the marked symbol of the slot (which can be seen in the slotMachine.txt)
    For slot1 (left), the character will be '$'
    For slot2 (middle), the character will be '%'
    For slot3 (right), the character will be '@'

Output:
strings of different symbols will be stored into the array input
*/
void createSlots(std::array<std::string, 30> &randomChars, char target);

/*
Function spinSlot, is used to show the spinning animation in each slot

Input:
1) randomChars: an array, that stored the strings of all different symbols ('N', 'C', 'R', 'E', 'L')

2) row: an integer, that stored the row of the marked symbol in the 1st parameter "map"
        (marked symbol, '$' / '%' / '@', which can be seen in the slotMachine.txt)

3) col: an integer, that stored the col of the marked symbol in the 1st parameter "map"

4) target: a character, that represents the marked symbol of the current slot

5) shifting: an integer, that stored the number of shifts occurred during the spinning of slot machine
    (for the visual effect of the spinning slot)

6) result: an character, that represents the result of the prize

7) endHead: a boolean, that check if the top of the slot matches the start of the printing letter
    (for centering the displayed letter in each slot)
    reference variable

8) endBottom: a boolean, that check if the bottom of the slot matches the end of the printing letter
    (for centering the displayed letter in each slot)
    reference variable

10) time: a boolean, that check if the elapsed time has reached a specified time
     if yes, then start stopping the slot (stop when the displayed result letter is centered)

Output:
changes the string elements of the 1st parameter
and prints out each change with TerminalUtils::printFrame
*/
void spinSlot(std::array<std::string, 30> randomChars, int row, int col, char target, int shifting, char result, bool &endHead, bool &endBottom, bool time);

/*
Function randomPull, is used to decide the prize for the player

No input

Output:
return a character, that represents the rarity of the prize
*/
char randomPull();

/*
Function runSlot, is used to find the positions of the marked symbols in different slots,
then perform all the spinning animation in the 3 slots of the slot machine,
together with the slowndown effect based on the elapsed time,
and stop the animation when the result letter has been centered in each slot

No input

Output:
return the output of function randomPull()
*/
char runSlot();

/*
Function openTreasure, is used to perform opening treasure animation after the spinning animation ends

Input:
the returned charcter from runSlot, that represents the prize to show the player

Output:
printing the treasure.txt, together with the centered texts that tell the player what does he get
*/
void openTreasure(char result, char symbol);


/*
Function opening, is used to perform the introduction animation before the spinning animation starts,
it also reads the key from the player to see if he wants to go back to the map / have a spin

Input:
coins: an integer, that stored the number of coins the player has
reference variable

Output:
a boolean that tells if the player have enough coins or not
*/
bool opening(int &coins);


/*
Function runSlotMachine, is used to call out all animations (introduction, slot machine & ending animations),
the atk of the player will be changed at the end based on the prize he gets

Input:
1) stat: an integer, that stored the current ATK/DEF/HP of the player
    reference variable

2) coins: an integer, that stored the number of coins the player has
    reference variable

3) symbol: an character, that stored the representing symbol of ATK (A) / DEF (D) / HP (H)

Output:
return the changed stat, and the remaining amount of coins
*/
void runSlotMachine(int &stat, int &coins, char symbol);


/*
Function chooseStat randomly select a stat, that will be buffed/ debuffed

Input:
1) ATK: an integer, that stored the current ATK of the player
    reference variable
    
2) DEF: an integer, that stored the current DEF of the player
    reference variable

3) HP: an integer, that stored the current HP of the player
    reference variable

4) coins: an integer, that stored the current number of coins of the player
    reference variable

Output:
no output, only the randomly selected stat will be buffed/ debuffed
*/
void chooseStat(int &ATK, int &DEF, int &HP, int &coins);


#endif
