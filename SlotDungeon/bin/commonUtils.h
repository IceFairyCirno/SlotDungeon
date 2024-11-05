#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cstdint>

#include "ansi.h"
#include "commonUtils.h"

class Logger
{
private:
  static std::string logFilePath;
  // Use for get file path
  static std::string getLogFilePath();
  // Use for get time
  static std::string getCurrentDateTime(const std::string &format);

public:
  /**
   * Logs a message to a predefined file with a timestamp. This function opens the
   * log file in append mode, ensuring that each message is added to the end of the
   * file without overwriting existing data. It prepends a timestamp to each message
   * for better traceability.
   *
   * @param message The text to be logged. This string is logged with a timestamp in the
   *                log file specified by the private member `logFilePath`.
   *
   * Usage:
   * - This method is used within a Logger class that manages logging operations.
   * - It is typically called whenever there is a need to log an event, error, or
   *   other significant occurrences in the application.
   *
   * Implementation Details:
   * - The function generates a timestamp using `getCurrentDateTime`, formatted
   *   according to the specified pattern (e.g., "[%Y-%m-%d %H:%M:%S]").
   * - It then attempts to open the designated log file stored in `logFilePath`.
   *   If the file cannot be opened, an error message is printed to standard error.
   * - If the file is successfully opened, the timestamp and message are written to
   *   the file, followed by a newline.
   * - The log file is closed after the message is written to ensure that the file
   *   is not corrupted and is accessible for future logs or other processes.
   *
   * Example Usage:
   * Logger logger;
   * logger.deleteOldLogs();
   * Logger::logMessage("[DEBUG] [main] Log Initialized");
   *
   * Note:
   * - Ensure that the logger is initialized with a valid log file path.
   * - Failure to open the log file results in an error message to `std::cerr`, and
   *   the function will return without logging the message.
   */
  static void logMessage(const std::string &message);
  // Logger::logMessage("[INFO] [file name] txt what's happend or what's wrong");

  // use once when game launch
  static void deleteOldLogs();
}; // class Logger

// Terminal utilities namespace to encapsulate functions
namespace TerminalUtils
{
  struct TerminalKeys
  {
    static constexpr int ESC = 27;   // TerminalUtils::TerminalKeys::ESC
    static constexpr int ENTER = 10; // TerminalUtils::TerminalKeys::ENTER
    static constexpr int SPACE = 32;
    static constexpr int UP = 1000;
    static constexpr int DOWN = 1001;
    static constexpr int RIGHT = 1002;
    static constexpr int LEFT = 1003;
    static constexpr int CTRL_LEFT = 2000;
    static constexpr int CTRL_RIGHT = 2001;
    static constexpr int SHIFT_LEFT = 2002;
    static constexpr int SHIFT_RIGHT = 2003;
    static constexpr int CTRL_UP = 2004;
    static constexpr int CTRL_DOWN = 2005;
    static constexpr int SHIFT_UP = 2006;
    static constexpr int SHIFT_DOWN = 2007;
    static constexpr int BACKSPACE = 127;
    static constexpr int CTRL_D = 4; // ASCII for Ctrl+D
    static constexpr int CTRL_A = 1; // ASCII for Ctrl+A
  };

  struct TerminalSize
  {
    int cols;
    int rows;
    bool operator==(const TerminalSize &other) const
    {
      return (cols == other.cols) && (rows == other.rows);
    }

    bool operator!=(const TerminalSize &other) const
    {
      return !(*this == other);
    }
  };

  // `currentRow` is used to keep track of the row position for certain terminal operations,
  // starting from the top row of the terminal window.
  static int currentRow = 1;

  /**
   * Retrieves the size of the terminal window.
   *
   * @return A TerminalSize structure containing two fields:
   *         - cols: The number of columns in the terminal.
   *         - rows: The number of rows in the terminal.
   */
  TerminalSize getTerminalSize();

  /**
   * Moves the cursor to a specified position within the terminal window.
   *
   * @param row The row number to move the cursor to (1-based indexing).
   * @param col The column number to move the cursor to (1-based indexing).
   */
  void moveCursorTo(int row, int col);

  /**
   * @brief Clears the current line where the cursor is located.
   *
   * @param mode Determines the clearing behavior:
   *             - 0 (default): Clears the entire line.
   *             - 1: Clears from the cursor position to the end of the line.
   */
  void clearLine(int mode = 0);

  // As the academic server dont have these file
  // - unicode/unistr.h
  // - unicode/uchar.h
  // So I had to use the map to store the wide characters
  /**
   * @brief Calculates the adjusted length of a given string, considering specific wide characters
   *        and optionally stripping ANSI color codes before calculating the length. This function
   *        is useful for aligning strings in terminals or other monospaced displays where some
   *        characters might be wider than others.
   *
   * @param text The input string whose adjusted length is to be calculated. This string
   *             may contain wide characters or ANSI color codes.
   * @param stripText If true, ANSI color codes within the string will be stripped before
   *                  length calculation. If false, the string is used as is.
   * @return The adjusted length of the string. This length accounts for wide characters,
   *         which are defined in the `knownWideChars` map. Each wide character or ANSI escape
   *         sequence is treated as a single character width for the purpose of alignment
   *         and display calculations.
   *
   * Known wide characters and certain emojis are specifically accounted for in the length
   * calculation, with each such character or sequence contributing a defined width to the
   * total adjusted length. This method ensures that text containing these characters is
   * properly aligned when displayed in environments that support variable-width characters.
   */
  int calculateAdjustedLength(const std::string &text, bool stripText = true);

  size_t utf8_char_length(unsigned char c);
  int getByteIndexFromVisualIndex(const std::string& rowContent, int visualIndex);

  static const std::unordered_set<std::string> knownWideChars = {
      "\xE2\x96\x87", // UTF-8 encoding for '▇'
      "\xE2\x96\x89", // UTF-8 encoding for '▉'
      "░",            // U+2591
      "▒",            // U+2592
      "▓",            // U+2593
      "█",            // U+2588
      "⡀",            // U+2800
      "⠁",            // U+2801
      "⠂",            // U+2802
      "⠃",            // U+2803
      "⠄",            // U+2804
      "⠅",            // U+2805
      "⠆",            // U+2806
      "⠇",            // U+2807
      "⠈",            // U+2808
      "⡈",            // U+2808
      "⠉",            // U+2809
      "⡉",            // U+2809
      "⠊",            // U+280A
      "⠋",            // U+280B
      "⠌",            // U+280C
      "⠍",            // U+280D
      "⠎",            // U+280E
      "⠏",            // U+280F
      "⡏",            // U+280F
      "⠐",            // U+2810
      "⠘",            // U+2818
      "⠙",            // U+2819
      "⠛",            // U+281B
      "⡛",            // U+281B
      "⠞",            // U+281E
      "⠟",            // U+282F
      "⠠",            // U+2820
      "⠻",            // U+283B
      "⠼",            // U+283C
      "⠿",            // U+283F
      "⠷",            // U+2837
      "⡁",            // U+2841
      "⡂",            // U+2842
      "⡃",            // U+2843
      "⡄",            // U+2844
      "⡅",            // U+2845
      "⡆",            // U+2846
      "⡇",            // U+2847
      "⡖",            // U+2856
      "⡘",            // U+2868
      "⡧",            // U+2867
      "⡯",            // U+286F
      "⡟",            // U+287F
      "⡿",            // U+287F
      "⢀",            // U+2880
      "⢁",            // U+2881
      "⢃",            // U+2883
      "⢋",            // U+288B
      "⢉",            // U+2889
      "⢡",            // U+28A1 (Assumed correct code as not provided in the original list)
      "⢹",            // U+2899
      "⢻",            // U+28BB
      "⢏",            // U+288F (Assumed correct code as not provided in the original list)
      "⢿",            // U+28BF
      "⢼",            // U+28BC
      "⣀",            // U+28C0
      "⣇",            // U+28C7
      "⣉",            // U+28C9
      "⣌",            // U+28CC
      "⣎",            // U+28CE
      "⣄",            // U+28C4
      "⣤",            // U+28E4
      "⣥",            // U+28E5
      "⣧",            // U+28E7
      "⣩",            // U+28E9
      "⣠",            // U+28E0
      "⣡",            // U+28E1
      "⣹",            // U+28F9
      "⣴",            // U+28F4
      "⣷",            // U+28F7
      "⣸",            // U+28F8
      "⣶",            // U+28F6
      "⣰",            // U+28F0
      "⣱",            // U+28F1
      "⣿",            // U+28FF
      "⣾",            // U+28FE
      "⣽",            // U+28FD
  };

  /**
   * Prints text to the terminal, optionally centering it and clearing the line first.
   * The text can be aligned left, centered, or right within the terminal window.
   *
   * @param text The string of text to be printed.
   * @param pos_row The row position for the text:
   *                - If -1 (default), the text is centered vertically in the terminal window.
   *                - If -2, the text is printed at `currentRow`.
   *                - If > 0, the text is printed at the specified row.
   * @param pos_col The horizontal alignment of the text:
   *                - -1: Align text to the left.
   *                - -2 (default): Center the text horizontally.
   *                - -3: Align text to the right.
   *                - Other values: Align text to the specified column.
   * @param clear_line If true (default), the line where the text is printed will be cleared first.
   */
  void printText(const std::string &text, int pos_row = -1, int pos_col = -2, bool clear_line = true);

  /**
   * Converts a specially formatted string to a long long integer value.
   * This function is particularly useful for interpreting custom-formatted
   * strings where leading '0's may indicate a negative value and the rest
   * of the string represents a large decimal number.
   *
   * @param binaryString A string representing a custom-formatted number.
   *                     The format expects leading '0's to indicate a negative
   *                     number and the remaining digits are treated as a decimal number.
   * @return The long long integer representation of the input string.
   */
  long long string_to_custom_int(const std::string &binaryString);

  /**
   * Aligns each string within a vector to the maximum length of the strings by adding spaces
   * evenly at the beginning and the end. This function ensures all strings have the same length,
   * which is particularly useful for creating visually aligned text blocks in console output or
   * text-based UIs.
   *
   * Usage:
   * - Pass a vector of strings that you wish to align.
   * - The function returns a new vector of strings, where each string is padded with spaces
   *   to match the length of the longest string in the input vector.
   *
   * Example:
   * std::vector<std::string> intro = {
   *     "Short line",
   *     "The longest line in the set",
   *     "Medium line"
   * };
   * std::vector<std::string> alignedIntro = alignTextToMaxLength(intro);
   * for (const auto& line : alignedIntro) {
   *     std::cout << '"' << line << '"' << std::endl;
   * }
   *
   * Output:
   * "       Short line       "
   * "The longest line in the set"
   * "      Medium line       "
   *
   * Note:
   * - The function aims for an even distribution of spaces. If the required number of spaces
   *   to be added is odd, the extra space is added at the end of the string.
   * - This function is ideal for preparing text for display in a fixed-width console or
   *   for text-based user interfaces where alignment improves readability.
   *
   * @param textLines A vector of strings to be aligned.
   * @return A new vector of strings, each padded with spaces to have the same length.
   */
  std::vector<std::string> alignTextToMaxLength(const std::vector<std::string> &textLines);

  /**
   * Prints a frame of text within the terminal, centered vertically and positioned horizontally
   * according to the specified parameters. This function handles various clearing behaviors to either
   * clear specific lines, the entire screen, or not at all, based on the value of force_clear.
   *
   * @param textLines A vector of strings where each string represents a line of text to be printed.
   *                  The text is vertically centered in the terminal window.
   * @param force_clear Controls the screen clearing behavior with several options:
   *                    0 - No clearing, each line is printed directly over whatever content was previously there.
   *                    1 - Clears each line before printing new content on that line.
   *                    2 - Clears the entire screen once before printing all lines. (ansi clear screen)
   *                    3 - Clears the entire screen once before printing all lines. (system clear screen)
   *                    >3 - Interprets the value as a bitmask where each bit corresponds to a line in textLines;
   *                         a bit set to '1' indicates that the specific line should be cleared before printing.
   * @param pos Specifies the horizontal positioning of the text:
   *             1 - Center the text horizontally within the terminal.
   *             2 - Print text aligned to the left of the terminal.
   *             3 - Print text aligned to the right of the terminal.
   *            Other values can be defined to allow different alignments, such as right-aligned text.
   *
   * Example usage:
   * TerminalUtils::printFrame({"Hello", "World"}, 2, 1);
   * // This clears the screen and prints "Hello" and "World" centered in the terminal.
   *
   * Special Notes:
   * - The vertical centering calculation may need adjustment if the number of text lines exceeds the terminal height.
   * - If the force_clear value specifies clearing more lines than there are in textLines (via bitmask),
   *   the function will only clear lines for which there is corresponding text.
   */
  void printFrame(const std::vector<std::string> &textLines, long long force_clear = 0, int pos = 1);

  // Below three are init terminal settings to use read key

  /**
   * Initializes the terminal for non-canonical mode input with no echo.
   * This function configures the terminal settings to immediately return input
   * without waiting for a newline, and disables echoing of input characters.
   * This behavior is suitable for applications that require real-time keystroke
   * processing in a way that typed characters are not displayed on the terminal.
   *
   * The input buffer is flushed at the beginning to ensure that no residual input
   * affects the program after the mode is switched. The original terminal settings
   * are saved before any modifications so they can be restored later, ensuring that
   * the terminal does not remain in an altered state after the program's execution.
   *
   * This function should be paired with `restoreTerminal`, which should be called
   * to revert the terminal to its original settings upon program completion or exit.
   * It is recommended to register `restoreTerminal` with `atexit()` to ensure that
   * the original settings are restored even if the program exits prematurely.
   *
   * Usage:
   * - Call `setupTerminal` at the start of the program or before initiating input
   *   processing that requires non-canonical mode or no-echo behavior.
   * - Register `restoreTerminal` with `atexit()` or ensure it is called manually
   *   before the program exits.
   *
   * Implementation Details:
   * - Utilizes POSIX termios functions to alter terminal attributes.
   * - Registers `restoreTerminal` with `atexit()` for automatic restoration of settings.
   * - Sets up signal handlers for SIGINT and SIGTERM to handle unexpected termination
   *   and ensure terminal settings are restored, preserving user terminal configuration.
   */
  void setupTerminal();

  /**
   * Handles signals sent to the program, ensuring a graceful shutdown by restoring the
   * terminal to its original settings. This function is specifically designed to respond
   * to signals such as SIGINT and SIGTERM, which typically indicate a request to interrupt
   * or terminate the program.
   *
   * Usage:
   * - This function should be registered as a signal handler after setting up the terminal
   *   for non-canonical mode input via `setupTerminal`. It ensures the terminal settings
   *   are restored to their original state, thereby maintaining the integrity of the user's
   *   terminal environment after program exit.
   *
   * Parameters:
   * - `signum`: The signal number representing the specific signal being handled. This
   *   includes common signals like SIGINT (interrupt from keyboard) and SIGTERM (termination
   *   request from the system).
   *
   * Implementation Notes:
   * - When a registered signal is received, `signalHandler` calls `restoreTerminal` to revert
   *   the terminal settings.
   * - It then forces the program to exit immediately using `_Exit`, passing the signal number
   *   as the exit status. `_Exit` is chosen over `exit` because it does not invoke functions
   *   registered with `atexit`, preventing potential re-entry issues with `restoreTerminal`
   *   if it's also registered with `atexit`.
   * - The handler should be registered early in the program to handle unexpected signals
   *   gracefully throughout the runtime.
   *
   * Example Registration:
   * ```cpp
   * signal(SIGINT, signalHandler);  // Register to handle interrupt signal
   * signal(SIGTERM, signalHandler); // Register to handle termination request
   * ```
   *
   * Note:
   * - This function can be adapted to handle additional signals by adding them to the
   *   registration process. This customization allows the program to respond appropriately
   *   to a wider range of termination conditions, enhancing robustness.
   */
  void signalHandler(int signum);

  /**
   * Restores the terminal to its original settings.
   * This function is designed to be called automatically upon program
   * exit or manually as needed to revert the terminal to the state it
   * was in before `setupTerminal` was called. It ensures that canonical
   * mode and echo are re-enabled, preserving the user's terminal
   * environment after the program completes.
   *
   * It is crucial to ensure this function is executed on program exit
   * to prevent leaving the terminal in a modified state, which could
   * affect subsequent terminal use.
   *
   * Usage:
   * - Automatically called on program exit if `setupTerminal` has been
   *   used, due to registration with `atexit`.
   * - Can also be called manually or from signal handlers to ensure
   *   terminal settings are restored after catching signals that would
   *   otherwise terminate the program abruptly.
   *
   * Implementation:
   * - Uses POSIX termios functions to restore saved terminal attributes.
   */
  void restoreTerminal();

  /**
   * Clears the terminal's input buffer by discarding any unread input. This ensures that
   * subsequent input read operations start with a clean state. It temporarily sets the
   * terminal to non-blocking mode to read and discard all available input, making it
   * especially useful before reading critical input to avoid unintended pre-buffered input.
   * Use with caution to prevent discarding legitimate input unintentionally.
   */
  void flushInputBuffer();

  /**
   * Reads a single key press from the terminal and returns an integer representing the key.
   * This function utilizes non-blocking input with specific handling for escape sequences
   * common in terminal interfaces, allowing it to detect and differentiate between various
   * special key combinations like arrow keys and modifier keys (e.g., Ctrl, Shift).
   *
   * Usage:
   * The function checks for input and returns immediately. It returns the ASCII value of
   * regular key presses, and predefined unique codes for special keys. If no key is pressed
   * within the specified timeout period, the function returns 0, making it suitable for
   * real-time applications where immediate response to input is needed.
   *
   * Example:
   * int key;
   * do {
   *     key = readKey();
   *     if (key != 0) {
   *         std::cout << "Key code: " << key << " pressed\n";
   *         if (key == TerminalKeys::UP) {
   *             std::cout << "Arrow Up was pressed\n";
   *         }
   *         // Additional handling for other keys
   *     }
   * } while (key != TerminalKeys::ESC); // Continue until ESC is pressed
   *
   * Special Return Values:
   * - 0: No key was pressed (within the terminal's timeout period).
   * - 27 (TerminalKeys::ESC): Escape key.
   * - TerminalKeys::UP, TerminalKeys::DOWN, TerminalKeys::LEFT, TerminalKeys::RIGHT: Arrow keys.
   * - TerminalKeys::CTRL_LEFT, TerminalKeys::CTRL_RIGHT, etc.: Modified arrow keys.
   * - TerminalKeys::CTRL_D: Ctrl+D (ASCII 4).
   * - TerminalKeys::CTRL_A: Ctrl+A (ASCII 1).
   *
   * @return An int representing the ASCII value of the key pressed or a unique integer
   *         code for special keys. Returns 0 if no key is pressed within the timeout period.
   */
  int readKey();
} // namespace TerminalUtils

namespace AnsiColors
{
  // extern std::unordered_map<std::string, std::string> colorMap;
  typedef struct
  {
    const char *Red;
    const char *Yellow;
    const char *Blue;
    const char *Black;
    const char *Cyan;
    const char *Green;
    const char *BRed;
    const char *BYellow;
    const char *BBlue;
    const char *BBlack;
    const char *BCyan;
    const char *BGreen;
    const char *rgb;
  } _color_map_t;
  extern const _color_map_t colorMap;

  extern std::unordered_map<std::string, std::string> colorMap_strMap;

  std::string generate_rgb_sequence(int r, int g, int b);
  
  std::string strippedText(const std::string &text);

  /**
   * Formats a string with ANSI color codes, allowing text and background coloring in the terminal output.
   * This function appends ANSI color codes to the beginning of the text and optionally appends a reset code at the end.
   *
   * @param txt The text to be formatted with color codes.
   * @param color The ANSI escape code for the text color (e.g., "\033[31m" for red).
   * @param bgColor The ANSI escape code for the background color (e.g., "\033[47m" for white background).
   * @param resetColor A boolean flag that indicates whether to append the ANSI reset code ("\033[0m")
   *                   at the end of the text. Setting this to true resets the color to the terminal's default
   *                   after the text, preventing the color from affecting text that follows.
   *
   * Example usage:
   * std::string redText = colorFormat("Warning!", "\033[31m", "", true);
   * // Outputs red "Warning!" and then resets to default terminal color.
   *
   * std::string alert = colorFormat("Error!", "\033[31m", "\033[47m", true);
   * // Outputs red text on a white background and resets the colors afterwards.
   *
   * Note:
   * - The color and bgColor parameters must be valid ANSI color codes. Improper codes may not render correctly.
   * - It's important to set resetColor to true if subsequent text should not be affected by the previous coloring.
   * - If bgColor is not needed, pass an empty string as the parameter.
   *
   * @return A formatted string with the applied ANSI color and background color codes, followed by the original text,
   *         and the reset code if resetColor is true.
   */
  std::string colorFormat(const std::string &txt,
                          const std::string &color = Ansi.fg.default_color,
                          const std::string &bgColor = Ansi.bg.default_color,
                          bool resetColor = true);
} // namespace AnsiColors

inline std::string trim(const std::string &str)
{
  size_t first = str.find_first_not_of(" \t\n\r\f\v");
  if (first == std::string::npos)
    return "";
  size_t last = str.find_last_not_of(" \t\n\r\f\v");
  return str.substr(first, (last - first + 1));
}

inline std::string rtrim(const std::string &str)
{
  size_t last = str.find_last_not_of(" \t\n\r\f\v");
  return str.substr(0, (last + 1));
}

#endif // COMMON_UTILS_H