#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "ansi.h"

const _ansi Ansi = {
    "\x1b[0m",
    // Clears a portion of the screen. If n is 0 (or absent), the area from the cursor position to the end of the screen is cleared. If n is 1, the area from the cursor position to the beginning of the screen is cleared.
    // If n is 2, the entire screen is cleared (in DOS ANSI.SYS, the cursor also moves to the upper left). If n is 3, the entire screen is cleared and all lines in the rollback buffer are deleted
    "\x1b[%dJ",
    // Clears a part of the line. If n is 0 (or absent), clears the area from the cursor position to the end of the line.
    // If n is 1, clears from the cursor position to the beginning of the line. If n is 2, the entire line is cleared. The cursor position remains unchanged.
    "\x1b[%dK",

    {
        "\x1b[30m",
        "\x1b[31m",
        "\x1b[32m",
        "\x1b[33m",
        "\x1b[34m",
        "\x1b[35m",
        "\x1b[36m",
        "\x1b[37m",
        "\x1b[90m",
        "\x1b[91m",
        "\x1b[92m",
        "\x1b[93m",
        "\x1b[94m",
        "\x1b[95m",
        "\x1b[96m",
        "\x1b[97m",
        "\x1b[38;2;%d;%d;%dm",
        "\x1b[39m",
    },
    {
        "\x1b[40m",
        "\x1b[41m",
        "\x1b[42m",
        "\x1b[43m",
        "\x1b[44m",
        "\x1b[45m",
        "\x1b[46m",
        "\x1b[47m",
        "\x1b[100m",
        "\x1b[101m",
        "\x1b[102m",
        "\x1b[103m",
        "\x1b[104m",
        "\x1b[105m",
        "\x1b[106m",
        "\x1b[107m",
        "\x1b[48;2;%d;%d;%dm",
        "\x1b[49m",
    },
    {
        "\x1b[1m",
        "\x1b[2m",
        "\x1b[3m",
        "\x1b[4m",
        "\x1b[5m",
        "\x1b[6m",
        "\x1b[7m",
        "\x1b[8m",
        "\x1b[9m",
        "\x1b[22m",
        "\x1b[10m",
        "\x1b[11m",
        "\x1b[12m",
        "\x1b[13m",
        "\x1b[14m",
        "\x1b[15m",
        "\x1b[16m",
        "\x1b[17m",
        "\x1b[18m",
        "\x1b[19m",
        "\x1b[21m",
        "\x1b[22m",
        "\x1b[23m",
        "\x1b[24m",
        "\x1b[25m",
        "\x1b[27m",
        "\x1b[28m",
        "\x1b[29m",
    },
    {
        "\x1b[%dA",
        "\x1b[%dB",
        "\x1b[%dC",
        "\x1b[%dD",
        "\x1b[%dE",
        "\x1b[%dF",
        "\x1b[%d;%dH",
        "\x1b[s",
        "\x1b[u",
        "\x1b[?25l",
        "\x1b[?25h",
        "\x1b[%dS",
        "\x1b[%dT",
    },
};

// Utility function for easy assembly of ANSI strings and output to standard output
void ansi(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

// initialization
void ansi_init()
{
// If it's a windows system
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

// Reset all settings
void ansi_reset()
{
  ansi(Ansi.reset);
}

void ansi_clear()
{
  ansi(Ansi.clear, 2);
}

void ansi_clear_to_start()
{
  ansi(Ansi.clear, 1);
}

void ansi_clear_to_end()
{
  ansi(Ansi.clear, 0);
}

void ansi_clear_line()
{
  ansi(Ansi.clear_line, 2);
}

void ansi_clear_to_line_start()
{
  ansi(Ansi.clear, 1);
}

void ansi_clear_to_line_end()
{
  ansi(Ansi.clear, 0);
}

void ansi_cursor_up()
{
  ansi(Ansi.cursor.up, 1);
}

void ansi_cursor_down()
{
  ansi(Ansi.cursor.down, 1);
}

void ansi_cursor_forward()
{
  ansi(Ansi.cursor.forward, 1);
}

void ansi_cursor_backward()
{
  ansi(Ansi.cursor.backward, 1);
}

void ansi_cursor_forward_n(int n)
{
  ansi(Ansi.cursor.forward, n);
}

void ansi_cursor_backward_n(int n)
{
  ansi(Ansi.cursor.backward, n);
}

void ansi_cursor_down_n(int n)
{
  ansi(Ansi.cursor.down, n);
}

void ansi_cursor_up_n(int n)
{
  ansi(Ansi.cursor.up, n);
}

void ansi_cursor_position(int row, int col)
{
  ansi(Ansi.cursor.position, row, col);
}

void ansi_cursor_save()
{
  ansi(Ansi.cursor.save);
}

void ansi_cursor_restore()
{
  ansi(Ansi.cursor.restore);
}

void ansi_cursor_hide()
{
  ansi(Ansi.cursor.hide);
}

void ansi_cursor_show()
{
  ansi(Ansi.cursor.show);
}

void ansi_fg_red()
{
  ansi(Ansi.fg.red);
}

void ansi_fg_green()
{
  ansi(Ansi.fg.green);
}

void ansi_fg_yellow()
{
  ansi(Ansi.fg.yellow);
}

void ansi_fg_blue()
{
  ansi(Ansi.fg.blue);
}

void ansi_fg_magenta()
{
  ansi(Ansi.fg.magenta);
}

void ansi_fg_cyan()
{
  ansi(Ansi.fg.cyan);
}

void ansi_fg_white()
{
  ansi(Ansi.fg.white);
}

void ansi_fg_default()
{
  ansi(Ansi.fg.default_color);
}

void ansi_fg_rgb(int r, int g, int b)
{
  ansi(Ansi.fg.rgb, r, g, b);
}

void ansi_bg_red()
{
  ansi(Ansi.bg.red);
}

void ansi_bg_green()
{
  ansi(Ansi.bg.green);
}

void ansi_bg_yellow()
{
  ansi(Ansi.bg.yellow);
}

void ansi_bg_blue()
{
  ansi(Ansi.bg.blue);
}

void ansi_bg_magenta()
{
  ansi(Ansi.bg.magenta);
}

void ansi_bg_cyan()
{
  ansi(Ansi.bg.cyan);
}

void ansi_bg_white()
{
  ansi(Ansi.bg.white);
}

void ansi_bg_rgb(int r, int g, int b)
{
  ansi(Ansi.bg.rgb, r, g, b);
}

void ansi_bg_default()
{
  ansi(Ansi.bg.default_color);
}

void ansi_font_bold()
{
  ansi(Ansi.font.bold);
}

void ansi_font_italic()
{
  ansi(Ansi.font.italic);
}

void ansi_font_underline()
{
  ansi(Ansi.font.underline);
}

void ansi_font_flash()
{
  ansi(Ansi.font.flash);
}

void ansi_font_flash_fast()
{
  ansi(Ansi.font.flash_fast);
}

void ansi_font_reverse()
{
  ansi(Ansi.font.reverse);
}

void ansi_font_invisible()
{
  ansi(Ansi.font.invisible);
}

void ansi_font_strikethrough()
{
  ansi(Ansi.font.strikethrough);
}

void ansi_font_normal()
{
  ansi(Ansi.font.normal);
}

void ansi_font_alt_1()
{
  ansi(Ansi.font.alt_1);
}

void ansi_font_alt_2()
{
  ansi(Ansi.font.alt_2);
}

void ansi_font_alt_3()
{
  ansi(Ansi.font.alt_3);
}

void ansi_font_alt_4()
{
  ansi(Ansi.font.alt_4);
}

void ansi_font_alt_5()
{
  ansi(Ansi.font.alt_5);
}

void ansi_font_alt_6()
{
  ansi(Ansi.font.alt_6);
}

void ansi_font_alt_7()
{
  ansi(Ansi.font.alt_7);
}

void ansi_font_alt_8()
{
  ansi(Ansi.font.alt_8);
}

void ansi_font_alt_9()
{
  ansi(Ansi.font.alt_9);
}

void ansi_font_alt_10()
{
  ansi(Ansi.font.alt_10);
}

void ansi_font_underline_double()
{
  ansi(Ansi.font.underline_double);
}

void ansi_font_bold_off()
{
  ansi(Ansi.font.bold_off);
}

void ansi_font_italic_off()
{
  ansi(Ansi.font.italic_off);
}

void ansi_font_underline_off()
{
  ansi(Ansi.font.underline_off);
}

void ansi_font_no_flash()
{
  ansi(Ansi.font.no_flash);
}

void ansi_font_no_reverse()
{
  ansi(Ansi.font.no_reverse);
}

void ansi_font_visible()
{
  ansi(Ansi.font.visible);
}

void ansi_font_no_strikethrough()
{
  ansi(Ansi.font.no_strikethrough);
}
