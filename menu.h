#ifndef __MENU_H_INCLUDED
#define __MENU_H_INCLUDED

class Menu : public App {
	const char *title;
	String *list;
	Screen &scr;
	Keyboard &kbd;
	int title_offset = 0;
	int sel_offset = 0;
public:
	int size = 0;
	int len = 0;
	int sel = 0;

	int w = COLS;
	int h = ROWS;
	int x = 0;
	int y = 0;

	Menu(Screen &scr, Keyboard &kbd, const char *title, int size);
	Menu(Screen &scr, Keyboard &kbd, const char *title, const char *items[]);
	~Menu();
	const char *selected() { return list[sel].c_str(); };
	void append(const char *text);
	void set(int nr, const char *text);
	void reset();
	int process();
	void show();
};
#endif
