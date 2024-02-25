#include <string.h>
#include "menu.h"

Menu::Menu(Screen &screen, Keyboard &keys, const char *t, int size) : scr(screen), kbd(keys), size(size), len(0), sel(0), title(t)
{
	list = new String[size];
}

Menu::Menu(Screen &screen, Keyboard &keys, const char *t, const char *items[]) : scr(screen), kbd(keys), size(size), len(0), sel(0), title(t)
{
	for (int i=0; items[i]; i++) {
		size ++;
		len ++;
	}
	list = new String[size];
	for (int i=0; i < len; i++)
		list[i] = String(items[i]);
}

void Menu::set(int nr, const char *text)
{
	if (nr >= size)
		return;
	list[nr] = String(text);
}

void Menu::append(const char *text)
{
	if (len >= size)
		return;
	list[len] = String(text);
	len ++;
}

void
Menu::show()
{
	scr.clear(0, 0, COLS, 1, scr.color(0, 128, 128));
	scr.text(0, 0, title, scr.color(255, 255, 0));
	int nr = (sel / (W-1))*(W-1);
	int y = 1;
	for (int pos = nr; pos < len && y < ROWS; pos ++) {
		if (pos == sel)
			scr.clear(0, y, COLS, 1, scr.color(0, 0, 255));
		else
			scr.clear(0, y, COLS, 1, 0);
		scr.text(0, y, list[pos].c_str());
		y ++;
	}
}

int
Menu::process()
{
	uint8_t c;
	int ret = -1;
	bool dirty = false;
	while ((c = kbd.input()) && ret == -1) {
		switch(c){
		case KEY_LEFT:
		case KEY_UP:
			sel --;
			dirty = true;
			break;
		case KEY_RIGHT:
		case KEY_DOWN:
			sel ++;
			dirty = true;
			break;
		case KEY_ESC:
			ret = -2;
			break;
		case KEY_MENU:
		case KEY_ENTER:
			ret = sel;
			break;
		default:
			break;
		}
	}
	sel = max(0, sel);
	sel = min(sel, len - 1);
	// scr.text(0, 16, String(sel).c_str());
	if (dirty)
		show();
	return ret;
}


Menu::~Menu()
{
	delete(list);
}
