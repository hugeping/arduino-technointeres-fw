#ifndef __EDIT_H_INCLUDED
#define __EDIT_H_INCLUDED
#include <Arduino.h>
#include "app.h"
#include "keys.h"
#include "screen.h"
#include "utf8.h"

class Edit : public App {
	const char *title;
	codepoint_t *buf;
	Screen &scr;
	Keyboard &kbd;
	boolean cur_visible;
	void visible();
public:
	boolean oneline = false;
	int x = 0;
	int y = 0;
	int w = COLS;
	int h = ROWS;
	int off = 0;
	int cur = 0;
	int size = 0;
	int len = 0;

	Edit(Screen &scr, Keyboard &kbd, const char *title, int size);
	Edit(Screen &scr, Keyboard &kbd, const char *title, const char *text);
	~Edit();
	void up();
	void down();
	void set(const char *text);
	int process();
	const char *text();
	void show();
	void geom(int x, int y, int w, int h);
};
#endif
