#include <chrono>
#include <cmath>
#include <cstdlib> // for atexit
#include <csignal> // for signal handling
#include <iostream>
#include <string>
#include <regex>
#include <thread>
#include <unordered_set>
#include <vector>
#include <cstddef> // for size_t
#include <cstdint> // for uint8_t

#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include "commonUtils.h"

namespace TerminalUtils
{

  TerminalSize getTerminalSize()
  {
    struct winsize w;
    // Use IOCTL to get the window size
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
    {
      std::cerr << "[Warning] Failed to get terminal size." << std::endl;
      return {0, 0}; // Return an empty size in case of failure
    }
    return {w.ws_col, w.ws_row};
  }

  void moveCursorTo(int row, int col)
  {
    // Move cursor to the specified row and column; note that terminal positions are 1-based
    std::cout << "\033[" << row << ";" << col << "H";
    std::cout.flush(); // Make sure to flush to apply the escape sequence immediately
  }

  void clearLine(int mode)
  {
    // Mode 0 : Clear entire line
    // Mode 1 : Clear from cursor to end of line
    if (mode == 0)
    {
      // Move cursor to the beginning of the line
      std::cout << "\033[1G";
    }
    // Clear from the cursor to the end of the line
    std::cout << "\033[K";
  }

  int calculateAdjustedLength(const std::string &text, bool stripText)
  {
    std::string stripped = stripText ? AnsiColors::strippedText(text) : text;

    // Using a simplified regex pattern for emojis
    std::regex emojiPattern(u8"[\U0001F600-\U0001F64F]");

    int adjustedLength = 0;
    size_t pos = 0;
    std::string remainingText;
    while (pos < stripped.length())
    {
      bool isWideCharOrEmoji = false;

      // Check for known wide characters first
      for (const auto &wc : knownWideChars)
      {
        if (stripped.substr(pos, wc.length()) == wc)
        {
          adjustedLength += 1; // Treat wide chars as single characters for alignment
          pos += wc.length();
          isWideCharOrEmoji = true;
          break; // Break the loop once a match is found
        }
      }

      // If not a known wide char, check for emojis
      if (!isWideCharOrEmoji)
      {
        std::smatch emojiMatch;
        remainingText = stripped.substr(pos);
        if (std::regex_search(remainingText, emojiMatch, emojiPattern, std::regex_constants::match_continuous))
        {
          adjustedLength += 1; // Treat emojis as single characters for alignment
          // pos += emojiMatch.str(0).length(); // Move past the entire emoji sequence
          pos += 2; // Move past the entire emoji sequence
          isWideCharOrEmoji = true;
        }
        // If neither wide char nor emoji, it's a standard character
        else
        {
          adjustedLength += 1;
          ++pos; // Move to the next character
        }
      }
    }
    return adjustedLength;
  }

  size_t utf8_char_length(unsigned char leadByte)
  {
    if (leadByte < 0x80)
      return 1;
    else if ((leadByte & 0xE0) == 0xC0)
      return 2;
    else if ((leadByte & 0xF0) == 0xE0)
      return 3;
    else if ((leadByte & 0xF8) == 0xF0)
      return 4;
    return 0; // Error in UTF-8 lead byte
  }

  // Returns the byte index in the UTF-8 string for the given visual index
  int getByteIndexFromVisualIndex(const std::string &str, int visualIndex)
  {
    size_t byteIndex = 0;
    int currentVisualIndex = 0;
    bool bool_pass = false;
    while (currentVisualIndex < visualIndex && byteIndex < str.length())
    {
      size_t charLength = utf8_char_length(static_cast<unsigned char>(str[byteIndex]));
      if (charLength == 0)
      {
        // Error in UTF-8 encoding
        Logger::logMessage("[ERROR] [terminalUtils/getByteIndexFromVisualIndex] Invalid UTF-8 encoding at byte position " + std::to_string(byteIndex));
        return -1;
      }
      byteIndex += charLength;
      if (charLength == 4)
      {
        currentVisualIndex += 2;
      }
      else
      {
        ++currentVisualIndex;
      }
    }
    // return (currentVisualIndex == visualIndex) ? byteIndex : -1;
    return byteIndex;
  }

  void printText(const std::string &text, int pos_row, int pos_col, bool clear_line)
  {
    // row = -1 default
    TerminalSize ts = getTerminalSize();
    int cols = ts.cols;
    int rows = ts.rows;

    // If no specific position is given for printing the text,
    // then it will be printed at the middle of the terminal
    // Calculate row position
    if (pos_row == -1) // Center vertically
      pos_row = rows / 2;
    else if (pos_row == -2)
      pos_row = currentRow;

    int visibleTextLength = calculateAdjustedLength(text);

    int start_col;
    switch (pos_col)
    {
    case -1: // Left align
      start_col = 1;
      break;
    case -2: // Center align
      start_col = (cols - visibleTextLength) / 2 + 1;
      break;
    case -3: // Right align
      start_col = cols - visibleTextLength + 1;
      break;
    default:
      start_col = pos_col;
      break;
    }

    start_col = std::max(start_col, 1); // Ensure start_col is positive

    // Clear the line if requested
    if (clear_line)
    {
      moveCursorTo(pos_row, 1); // Move to the beginning of the line
      clearLine();
    }

    // Move cursor to start_col; note that terminal columns are 1-based
    moveCursorTo(pos_row, start_col);

    // Print the text
    std::cout << text;
    std::cout.flush(); // Make sure to flush to apply the escape sequence immediately

    // Update currentRow after printing
    currentRow++;
  }

  long long string_to_custom_int(const std::string &binaryString)
  {
    if (binaryString.empty())
    {
      // std::cerr << "Invalid input: string is empty.\n";
      Logger::logMessage("[ERROR] [terminalUtilsstring_to_custom_int] Invalid input: string is empty.");
      return 0;
    }
    // Determine if the string indicates a negative number by leading zeros.
    bool isNegative = !binaryString.empty() && binaryString[0] == '0';

    // The resulting value, initially 0.
    long long result = 0;

    // Start processing from the first '1' to ignore leading '0's.
    size_t firstOne = binaryString.find('1');
    if (firstOne == std::string::npos)
    {
      result = (binaryString.length() - 1) * 10;
      if (isNegative)
        result = -result;
      return result;
    }

    // Extract the part of the string after leading zeros (if any).
    std::string significantPart = binaryString.substr(firstOne);

    // Convert the significant part of the string to a number.
    for (char digit : significantPart)
    {
      result = result * 10 + (digit - '0');
    }
    result += pow(10, (binaryString.length() - 1));

    // Apply negation if the string was determined to be negative.
    if (isNegative)
    {
      result *= -1;
    }

    return result;
  }

  std::vector<std::string> alignTextToMaxLength(const std::vector<std::string> &textLines)
  {
    size_t max_length = 0;
    // Find the maximum length of strings in the vector
    for (const auto &line : textLines)
    {
      if (line.length() > max_length)
      {
        max_length = line.length();
      }
    }

    std::vector<std::string> alignedLines;
    for (const auto &line : textLines)
    {
      size_t totalSpacesToAdd = max_length - line.length();
      size_t spacesAtStart = totalSpacesToAdd / 2;
      size_t spacesAtEnd = totalSpacesToAdd - spacesAtStart;
      std::string newLine = std::string(spacesAtStart, ' ') + line + std::string(spacesAtEnd, ' ');
      alignedLines.push_back(newLine);
    }

    return alignedLines;
  }

  void printFrame(const std::vector<std::string> &textLines, long long force_clear, int pos)
  {
    // Assuming getTerminalSize and printText are member functions of TerminalUtils.
    TerminalUtils::TerminalSize ts = TerminalUtils::getTerminalSize();
    int rows = ts.rows;

    // Clear screen if force_clear flag indicates
    if (force_clear == 2)
    {
      std::cout << "\033[2J\033[H"; // ANSI escape codes to clear screen and move cursor to home position
    }
    else if (force_clear == 3)
    {
      system("clear");
    }

    // Calculate starting row to center text vertically if not specified
    currentRow = (rows - textLines.size()) / 2;

    if (pos == 1 || pos == 2 || pos == 3)
    {
      pos = -pos;
    }
    if (pos == -1)
    {
      pos = -2;
    }
    else if (pos == -2)
    {
      pos = -1;
    }

    // If force_clear > 2, assume it's a binary mask for clearing lines individually
    // This part is speculative and assumes force_clear's binary representation controls line clearing
    if (force_clear > 2 || force_clear < 0)
    {
      std::string binaryString;
      if (force_clear > 0)
        binaryString = std::to_string(force_clear);
      else
        binaryString = "0" + std::to_string(force_clear * -1).substr(1);
      for (size_t i = 0; i < textLines.size(); ++i)
      {
        bool clearLine = (i < binaryString.length()) ? binaryString[i] == '1' : false;
        TerminalUtils::printText(textLines[i], -2, pos, clearLine);
      }
    }
    else
    {
      // For any other force_clear value, use it directly to determine clearing
      for (const auto &line : textLines)
      {
        TerminalUtils::printText(line, -2, pos, force_clear == 1);
      }
    }

    // Move cursor to bottom for better interface
    TerminalUtils::moveCursorTo(rows, 1); // Assuming the intent was to move the cursor to the bottom left
  }

  static struct termios oldt;

  // Setup for terminal keyboard input handling
  void setupTerminal()
  {
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt); // Save and get current terminal attributes
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and echo
    newt.c_cc[VMIN] = 0;                     // Minimum number of characters for noncanonical read
    newt.c_cc[VTIME] = 0;                    // Timeout in tenths of a second for noncanonical read (0.1 seconds)
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Set new attributes
    ansi(Ansi.cursor.hide);
    Logger::logMessage("[INFO] [init] setupTerminal successful");
    tcflush(STDIN_FILENO, TCIFLUSH);

    // Ensure the terminal is restored when the program exits normally
    atexit(restoreTerminal);

    // Handle signals that may terminate the program
    signal(SIGINT, signalHandler);  // Interrupt signal (Ctrl-C)
    signal(SIGTERM, signalHandler); // Termination request
                                    // Add other signals as necessary

    std::locale::global(std::locale("en_US.UTF-8")); // Adjust the locale as needed
    std::wcout.imbue(std::locale());                 // Make wcout use the global locale
  }

  void restoreTerminal()
  {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    ansi(Ansi.cursor.show);
    // std::cout << "\033[?25h"; // Show cursor using ANSI escape code
    std::cout.flush(); // Make sure to flush to apply the escape sequence immediately
    Logger::logMessage("[INFO] Terminal settings restored");
  }

  // Signal handler that restores terminal settings
  void signalHandler(int signum)
  {
    restoreTerminal();
    // Exit program after handling signal
    _Exit(signum);
  }

  int readKey()
  {
    fd_set read_fds;
    struct timeval timeout;
    int retval;

    // Set up the file descriptor set.
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    // Set up the timeout for the initial read.
    timeout.tv_sec = 0;
    timeout.tv_usec = 200; // Initially very short to detect standalone ESC

    retval = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

    if (retval == -1)
    {
      perror("select()");
      return -1; // Error occurred
    }
    else if (retval)
    {
      // Data is available, read it.
      unsigned char ch;
      read(STDIN_FILENO, &ch, 1);

      if (ch == 27)
      { // Escape character
        // Use a very short timeout to check if more characters follow
        timeout.tv_usec = 100; // Short timeout to distinguish sequences
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        retval = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

        if (retval == 0)
        {
          return TerminalKeys::ESC;
        }
        else
        {
          // More characters follow, read the sequence
          unsigned char seq[6];
          int num_read = read(STDIN_FILENO, seq, sizeof(seq) - 1);
          seq[num_read] = '\0'; // Null terminate to make it a string

          // Handle known sequences
          if (num_read > 0 && seq[0] == '[')
          { // Ensure that there's a '[' follow-up
            if (strcmp((const char *)seq, "[A") == 0)
              return TerminalKeys::UP;
            if (strcmp((const char *)seq, "[B") == 0)
              return TerminalKeys::DOWN;
            if (strcmp((const char *)seq, "[C") == 0)
              return TerminalKeys::RIGHT;
            if (strcmp((const char *)seq, "[D") == 0)
              return TerminalKeys::LEFT;
            if (strcmp((const char *)seq, "[1;5D") == 0)
              return TerminalKeys::CTRL_LEFT;
            if (strcmp((const char *)seq, "[1;5C") == 0)
              return TerminalKeys::CTRL_RIGHT;
            if (strcmp((const char *)seq, "[1;2D") == 0)
              return TerminalKeys::SHIFT_LEFT;
            if (strcmp((const char *)seq, "[1;2C") == 0)
              return TerminalKeys::SHIFT_RIGHT;
            if (strcmp((const char *)seq, "[1;5A") == 0)
              return TerminalKeys::CTRL_UP;
            if (strcmp((const char *)seq, "[1;5B") == 0)
              return TerminalKeys::CTRL_DOWN;
            if (strcmp((const char *)seq, "[1;2A") == 0)
              return TerminalKeys::SHIFT_UP;
            if (strcmp((const char *)seq, "[1;2B") == 0)
              return TerminalKeys::SHIFT_DOWN;
          }
          else
          {
            return TerminalKeys::ESC; // Unhandled escape sequence
          }
        }
      }
      else if (ch == TerminalKeys::CTRL_D)
      {
        return TerminalKeys::CTRL_D; // Handle Ctrl+D
      }
      else if (ch == TerminalKeys::CTRL_A)
      {
        return TerminalKeys::CTRL_A; // Handle Ctrl+A
      }
      return ch; // Return normal characters and unhandled control sequences
    }
    else
    {
      // No data within the timeout period
      return 0;
    }
  }

} // namespace TerminalUtils