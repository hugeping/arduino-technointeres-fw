#ifndef __MENU_H_INCLUDED
#define __MENU_H_INCLUDED
#include <Arduino.h>
#include "app.h"
#include "keys.h"
#include "screen.h"

class Menu : public App {
	const char *title;
	String *list;
	Screen &scr;
	Keyboard &kbd;
public:
	int size;
	int len;
	int sel;

	Menu(Screen &scr, Keyboard &kbd, const char *title, int size);
	Menu(Screen &scr, Keyboard &kbd, const char *title, const char *items[]);
	~Menu();
	void append(const char *text);
	void set(int nr, const char *text);
	int process();
	void show();
};
#endif
