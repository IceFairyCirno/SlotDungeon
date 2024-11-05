# Slot Dungeon: The Casino Caverns
Defeat the king and get the treasure! <br />
ENGG 1340 Group Project (Group 35)


## How to launch the game:
### Lazy method (pre made)
 1. [Click here](https://github.com/DickyLing/ENGG1340-Group-Project/releases/tag/0.0.1) to download the newest version of the game.
 2. Unzip it into a folder
 3. Type "./launcher" in Command Prompt (Make sure you can run the 'launcher' with permission to execute)

### Source code
 1. [Click here](https://github.com/DickyLing/ENGG1340-Group-Project/releases/tag/0.0.1) to download source code of the newest version of the game.
 2. Unzip it into a folder
 3. Type "make" in Command Prompt
 3. Type "./launcher" in Command Prompt

## Features
### Emoji 
  1. We used emoji to represent various elements that would make coding more difficult
### Battle system 
  1. Enemies that use skills based on probability+player health+enemy health
  2. Smooth Skill Animation
### Read file
  1. Read file used in settings.cpp and the gameData.cpp while gameData.cpp encoded
  2. Output file to update data and store for next time
### In-code document
  1. commonUtils.h
  2. slotMachine.h
### Slot Machine
  1. Smooth spinning animation
  2. including the random generation of prize and the random selection of buffed/ debuffed status
  3. interesting opening interaction and ending open treasure animation
### Using launcher and main
  1. Allow additional program to run before run main
  2. Main handle the whole game progress, allow more center control


## Manual change settings
 1. Open config.ini
 2. Change it
 3. Save it
 Remark: Changing last level won't help you skip :)


## Mod!
 1. There is instruction of showing the format and detail of the additional art and map
 2. Current only map.txt, menu.txt, facilities.txt
 3. Enjoin showing off your art or customize your art!


## Skip or add value (cheats?)
### Skip level / add inventory
 1. In gameData.h change it
 2. Deleted bin/gameStats.dat
 3. (Optional) Change config.ini lastlevel to the level you change in gameData.h (so when continue you are the last level)
 4. Reopen game
### Change enemy data
 1. Change in content/Data/enemyData.txt
 2. Save and reopen game

## Authors

- [Ng Ka Yiu UID: 3036270546](https://github.com/CyberKittenLoL)
- [Ling Chun Ho UID: 3036224054](https://github.com/DickyLing)
- [Wei Sui Fung UID: 3036224779](https://github.com/kkwei0331)
- [Xu Jia Luo UID: 3036135722](https://github.com/goldentrouser)
- [Lam King Wang UID: 3036269509](https://github.com/GeziILam)


## Reference
treasure.txt has used the ASCII art from https://ascii.co.uk/art/treasure <br />
opening.txt has used the ASCII art from https://steamcommunity.com/groups/asciiartamalgamation/discussions/1/3008934419468790530/?ctp=13 <br />
ansi.h references https://github.com/noxue/ansi-escape-lib


