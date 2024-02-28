#ifndef __VIEW_H_INCLUDED
#define __VIEW_H_INCLUDED
#include <Arduino.h>
#include "app.h"
#include "keys.h"
#include "screen.h"
#include "utf8.h"

class View : public App {
	const char *title;
	codepoint_t *buf;
	Screen &scr;
	Keyboard &kbd;
	boolean visible = false;
public:
	int x = 0;
	int y = 0;
	int w = COLS;
	int h = ROWS;
	int off = 0;
	int size = 0;
	int len = 0;

	View(Screen &scr, Keyboard &kbd, const char *title);

	~View();
	void set(const char *text);
	void up();
	void down();
	int process();
	void show();
};
#endif
