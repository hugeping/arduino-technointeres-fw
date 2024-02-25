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
	int curx;
	int cury;
	int size;
	int len;
public:
	Edit(Screen &scr, Keyboard &kbd, const char *title, int size);
	Edit(Screen &scr, Keyboard &kbd, const char *title, const char *text);
	~Edit();
	void set(const char *text);
	int process();
	void show();
};
#endif
