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
public:
	int x;
	int y;
	int w;
	int h;
	int off;
	int size;
	int len;

	View(Screen &scr, Keyboard &kbd, const char *title);

	~View();
	void set(const char *text);
	void up();
	void down();
	int process();
	void show();
};
#endif
