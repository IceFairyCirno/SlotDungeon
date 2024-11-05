#include <iostream>
#include <regex>
#include <cwchar>
#include <stdio.h>
#include <stdlib.h>

#include "commonUtils.h"

namespace AnsiColors
{
  const _color_map_t colorMap
  {
    Ansi.fg.red,
    Ansi.fg.yellow,
    Ansi.fg.blue,
    Ansi.fg.black,
    Ansi.fg.cyan,
    Ansi.fg.green,
   
    Ansi.fg.bred,
    Ansi.fg.byellow,
    Ansi.fg.bblue,
    Ansi.fg.bblack,
    Ansi.fg.bcyan,
    Ansi.fg.bgreen,
    "rgb",
  };

  std::unordered_map<std::string, std::string> colorMap_strMap = {
      {"Red", Ansi.fg.red},
      {"Yellow", Ansi.fg.yellow},
      {"Blue", Ansi.fg.blue},
      {"Black", Ansi.fg.black},
      {"Cyan", Ansi.fg.cyan},
      {"Green", Ansi.fg.green},

      {"BRed", Ansi.fg.bred},
      {"BYellow", Ansi.fg.byellow},
      {"BBlue", Ansi.fg.bblue},
      {"BBlack", Ansi.fg.bblack},
      {"BCyan", Ansi.fg.bcyan},
      {"BGreen", Ansi.fg.bgreen},
      {"rgb", "rgb"}
  };

  std::string generate_rgb_sequence(int r, int g, int b)
  {
    return "\x1b[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
  }

  // Function to calculate the visible length of a string excluding ANSI codes
  std::string strippedText(const std::string &text)
  {
    std::regex ansiCode("\\x1B\\[[0-9;]*m");
    std::string stripped = std::regex_replace(text, ansiCode, "");
    return stripped;
  }

  std::string colorFormat(const std::string &txt, const std::string &color, const std::string &bgColor, bool resetColor)
  {
    std::string resetCode = resetColor ? Ansi.reset : "";
    return color + bgColor + txt + resetCode;
  }
}