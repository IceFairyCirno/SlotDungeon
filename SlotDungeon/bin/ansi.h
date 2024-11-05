// A file that simplifies the ansi code

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef __ANSI_H__
#define __ANSI_H__

typedef struct
{
  const char *black;    // ansi(Ansi.fg.black);
  const char *red;      // ansi(Ansi.fg.red);
  const char *green;    // ansi(Ansi.fg.green);
  const char *yellow;   // ansi(Ansi.fg.yellow);
  const char *blue;     // ansi(Ansi.fg.blue);
  const char *magenta;  // ansi(Ansi.fg.magenta);
  const char *cyan;     // ansi(Ansi.fg.cyan);
  const char *white;    // ansi(Ansi.fg.white);
  const char *bblack;   // ansi(Ansi.fg.bblack);
  const char *bred;     // ansi(Ansi.fg.bred);
  const char *bgreen;   // ansi(Ansi.fg.bgreen);
  const char *byellow;  // ansi(Ansi.fg.byellow);
  const char *bblue;    // ansi(Ansi.fg.bblue);
  const char *bmagenta; // ansi(Ansi.fg.bmagenta);
  const char *bcyan;    // ansi(Ansi.fg.bcyan);
  const char *bwhite;   // ansi(Ansi.fg.bwhite);
  const char *rgb;      // ansi(Ansi.fg.rgb, 255, 0, 0);
  const char *default_color;
} _color_t;

typedef struct
{
  const char *bold;             // ansi(Ansi.font.bold);
  const char *dim;              // ansi(Ansi.font.dim);
  const char *italic;           // ansi(Ansi.font.italic);
  const char *underline;        // ansi(Ansi.font.underline);
  const char *flash;            // ansi(Ansi.font.flash);
  const char *flash_fast;       // ansi(Ansi.font.flash_fast);
  const char *reverse;          // ansi(Ansi.font.reverse);
  const char *invisible;        // ansi(Ansi.font.invisible);
  const char *strikethrough;    // ansi(Ansi.font.strikethrough);
  const char *normal;           // ansi(Ansi.font.normal);
  const char *alt_1;            // ansi(Ansi.font.alt_1);
  const char *alt_2;            // ansi(Ansi.font.alt_2);
  const char *alt_3;            // ansi(Ansi.font.alt_3);
  const char *alt_4;            // ansi(Ansi.font.alt_4);
  const char *alt_5;            // ansi(Ansi.font.alt_5);
  const char *alt_6;            // ansi(Ansi.font.alt_6);
  const char *alt_7;            // ansi(Ansi.font.alt_7);
  const char *alt_8;            // ansi(Ansi.font.alt_8);
  const char *alt_9;            // ansi(Ansi.font.alt_9);
  const char *alt_10;           // ansi(Ansi.font.alt_10);
  const char *underline_double; // ansi(Ansi.font.underline_double);
  const char *bold_off;         // ansi(Ansi.font.bold_off);
  const char *italic_off;       // ansi(Ansi.font.italic_off);
  const char *underline_off;    // ansi(Ansi.font.underline_off);
  const char *no_flash;         // ansi(Ansi.font.no_flash);
  const char *no_reverse;       // ansi(Ansi.font.no_reverse);
  const char *visible;          // ansi(Ansi.font.visible);
  const char *no_strikethrough; // ansi(Ansi.font.no_strikethrough);
} _font_t;

typedef struct
{

  const char *up;          // ansi(Ansi.cursor.up, 1);
  const char *down;        // ansi(Ansi.cursor.down, 1);
  const char *forward;     // ansi(Ansi.cursor.forward, 1);
  const char *backward;    // ansi(Ansi.cursor.backward, 1);
  const char *next_line;   // ansi(Ansi.cursor.next_line, 1);
  const char *prev_line;   // ansi(Ansi.cursor.prev_line,  1);
  const char *position;    // ansi(Ansi.cursor.position, 5, 3);
  const char *save;        // ansi(Ansi.cursor.save);
  const char *restore;     // ansi(Ansi.cursor.restore);
  const char *hide;        // ansi(Ansi.cursor.hide);
  const char *show;        // ansi(Ansi.cursor.show);
  const char *scroll_up;   // ansi(Ansi.cursor.scroll_up, 1);
  const char *scroll_down; // ansi(Ansi.cursor.scroll_down, 1);
} _cursor_t;

typedef struct
{
  const char *reset;
  const char *clear;      // ansi(Ansi.clear,2) ;
  const char *clear_line; // ansi(Ansi.hide);
  _color_t fg;
  _color_t bg;
  _font_t font;
  _cursor_t cursor;
} _ansi;

extern const _ansi Ansi;

#ifdef __cplusplus
// C++
extern "C"
{
#endif

  // Utility function to easily assemble ANSI strings and output to standard output
  void ansi(const char *fmt, ...);

  // Initialize, clear screen
  void ansi_init();

  // Reset all settings
  void ansi_reset();

  // Clear screen, cursor position unchanged
  void ansi_clear();

  // Clear the cursor to the beginning of the screen
  void ansi_clear_to_start();

  // Clear the cursor to the end of the screen
  void ansi_clear_to_end();

  // Clear the entire line
  void ansi_clear_line();

  // Clears the cursor to the beginning of the line
  void ansi_clear_to_line_start();

  // Clear cursor to end of line
  void ansi_clear_to_line_end();

  //=======CURSOR Operation=====================

  // Move the cursor up one line.
  void ansi_cursor_up();

  // Cursor moves to next line
  void ansi_cursor_down();

  // Cursor one column to the right
  void ansi_cursor_forward();

  // Cursor one column to the left
  void ansi_cursor_backward();

  // Cursor moves forward n columns
  void ansi_cursor_forward_n(int n);

  // Cursor moves back n columns
  void ansi_cursor_backward_n(int n);

  // Cursor moves down n lines
  void ansi_cursor_down_n(int n);

  // Cursor moves up n lines
  void ansi_cursor_up_n(int n);

  // Move the cursor to the specified row or column
  void ansi_cursor_position(int row, int col);

  // Save Cursor Position
  void ansi_cursor_save();

  // Restore cursor position
  void ansi_cursor_restore();

  // Hide Cursor
  void ansi_cursor_hide();

  // Show Cursor
  void ansi_cursor_show();

  //==========Foreground color==========

  void ansi_fg_red();

  void ansi_fg_green();

  void ansi_fg_yellow();

  void ansi_fg_blue();
  // magenta > color between red and purple
  void ansi_fg_magenta();
  // cyan > color between blue and green
  void ansi_fg_cyan();

  void ansi_fg_white();

  // Restore default colors
  void ansi_fg_default();

  // Set the background color to the specified rgb color
  void ansi_fg_rgb(int r, int g, int b);

  //=======Background color=====

  void ansi_bg_red();

  void ansi_bg_green();

  void ansi_bg_yellow();

  void ansi_bg_blue();

  void ansi_bg_magenta();

  void ansi_bg_cyan();

  void ansi_bg_white();

  void ansi_bg_default();

  void ansi_bg_rgb(int r, int g, int b);

  //=====Font======

  // BOLD
  void ansi_font_bold();

  // Italic
  void ansi_font_italic();

  // Underline
  void ansi_font_underline();

  // Flash
  void ansi_font_flash();

  // font fast flash, not all unsupported, most of the time and ansi_font_flash effect is the same
  void ansi_font_flash_fast();

  // ** Invert font color and background color **
  void ansi_font_reverse();

  // Hide content
  void ansi_font_invisible();

  // strikethrough
  void ansi_font_strikethrough();

  // Restore default fonts
  void ansi_font_normal();

  // Optional fonts, not useful
  void ansi_font_alt_1();

  void ansi_font_alt_2();

  void ansi_font_alt_3();

  void ansi_font_alt_4();

  void ansi_font_alt_5();

  void ansi_font_alt_6();

  void ansi_font_alt_7();

  void ansi_font_alt_8();

  void ansi_font_alt_9();

  void ansi_font_alt_10();

  // =======Canceling Specific Settings=======

  // Double underline
  void ansi_font_underline_double();

  // Unbold
  void ansi_font_bold_off();

  // Cancel italics
  void ansi_font_italic_off();

  // Cancel underlining
  void ansi_font_underline_off();

  // Cancel blinking
  void ansi_font_no_flash();

  // Cancel color and background color inversion
  void ansi_font_no_reverse();

  // Display content
  void ansi_font_visible();

  // Cancel strikethrough
  void ansi_font_no_strikethrough();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __ANSI_H__
