#ifndef __MENU_H_INCLUDED
#define __MENU_H_INCLUDED
#include "keys.h"
#include "screen.h"

class Menu {
	const char *title;
	char **list;
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
	void set(int nr, char *text);
	int process();
	void show();
};
#endif
