# Project Function List

## FileUtils.cpp

Include with: `#include "fileUtils.h"`

### Functions

- **`readLinesFromFile(filepath)`**
  - **Input:** `string filepath`
  - **Return:** Vector of strings with lines from file

- **`ltrim(s)`, `rtrim(s)`, `trim(s)`**
  - **Input:** `string s`
  - **Return:** String with removed whitespace (leading for `ltrim`, trailing for `rtrim`, both for `trim`)

- **`readAsciiArt(filename, artname)`**
  - **Input:** Two strings `filename` and `artname` (filename is txt file name like menu.txt map.txt example "map", "map0" i.e., map0 in map.txt)
  - **Return:** Vector of strings with lines from `artname` as elements

## ColorTextUtils.cpp

Include with: `#include "commonUtils.h"`

### Functions

- **`strippedText(text)`**

  - **Input:** `const std::string &text` - The string from which ANSI escape codes are to be removed.
  - **Return:** A `std::string` that has had all ANSI escape codes stripped from it.

- **`colorFormat(txt, color, bgColor, resetColor)`**
  - **Input:** `string txt`, `color`, `bgColor`, and `bool resetColor`
  - **Return:** String with assigned text color and background color. The `resetColor` parameter determines if the color change is permanent.

## TerminalUtils.cpp

Include with: `#include "commonUtils.h"`

### Functions

- **`getTerminalSize()`**
  - **Return:** Array with two elements (int): height (col), width (row)

- **`moveCursor(row, col)`**
  - **Input:** `int row`, `col` (1-based)
  - **Do:** Move cursor to given row and column

- **`cleanLine(mode)`**
  - **Input:** `int mode` (0=clean entire line, 1=clean from cursor to the end of the line)

- **`centerText(text, row, clean_line)`**
  - **Input:** `string text`, `int row`, `bool clean_line`
  - **Do:** Center the text in the middle of the screen

- **`printFrame(textLines, pos)`**
  - **Input:** `vector<string> textLines`, `int pos`
  - **Do:** Print out the frame (map, menu) and center it

- **`setupTerminal()` / `restoreTerminal()`**
  - **Do:** Disable/Enable canonical mode and echo

- **`readKey()`**
  - **Return:** User's last input key (ascii)
  - **Example:**
    ```cpp
    int c;
    do {
        c = readKey();
    } while (c == 0);
    if (c == 'W') std::cout << "W has been pressed\n";
    ```


## TimeUtils.cpp

Include with: `use: timeUtils::functionName`

### Functions

- **`getCurrentDateTime()`**
  - **Return:** String with date and time (e.g., "Wed Oct 10 12:23:45 2024")

## Settings.cpp

### Functions

- **`loadSettings()`, `saveSettings()`, `showSettings()`**
  - **Do:** Operations related to settings (incomplete)

## PlayerMove.cpp

### Functions

- **`movePlayer(display_items, playerRow, playerCol, key)`**
  - **Input:** `vector<string> display_item`, `int playerRow`, `playerCol`, `key` (player moving direction in ascii)
  - **Do:** Move player

- **`main()`**
  - **Do:** Move player, print centered map

## Menu.cpp

Include with: `#include "menu.h"`

### Functions

- **`Menu()`**
  - **init:** Store vector of map template at `artPieces` and return error message if art not found
  - **loop:** Allow user to choose a choice and go to different functions e.g., start game

- **`isError()`, `displayMenu()`, `handleUserChoice(force_n)`, `selection()`, `run(errorMessage)`**
  - **Various inputs and operations** related to menu handling and user interaction

## Main.cpp

### Functions

- **`printCWD()`**
  - **Do:** Add the current directory to a log

- **`main()`**
  - **Do:** Initialization and logging

## Logger.cpp

Include with: `#include "commonUtils.h"`

### Functions

- **init** `deleteOldLogs()`
- **`getLogFilePath()`, `getCurrentDateTime(format)`, `logMessage(message)`**
  - **Various operations** related to logging and log file management
