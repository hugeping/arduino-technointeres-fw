#ifndef __SCREEN_H_INCLUDED
#define __SCREEN_H_INCLUDED

#include <TFT_eSPI.h>
#include <SPI.h>
#include "font8x10.h"
#include "utf8.h"

#define W 240
#define H 240

typedef uint16_t color_t;

static const struct font8 *font = &font8x10;

#define COLS (W/FONT_W)
#define ROWS (H/FONT_H)

typedef struct {
	codepoint_t glyph;
	color_t bg;
	color_t fg;
} cell_t;

static cell_t screens[2][ROWS*COLS];

class Screen
{
	const uint8_t* lookup_glyph(codepoint_t cp);
	cell_t *get_screen();
	cell_t *get_old_screen();
	void update_cell(int x, int y, cell_t *cell);
public:
	TFT_eSPI tft;
	Screen();
	void setup();
	void draw_text(int x, int y, const char *str, color_t col = 0xffff);
	void draw_glyph(int x, int y, codepoint_t cp, color_t col = 0xffff);
	void cell(int x, int y, codepoint_t cp, color_t fg = 0xffff);
	void clear(int xx = 0, int yy = 0, int w = COLS, int h = ROWS, color_t col = 0x0);
	void text(int x, int y, const char *str, color_t fg = 0xffff);
	void update();
	color_t color(byte r, byte g, byte b);
};

//extern Screen *gscr;

#endif
