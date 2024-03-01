#include "external.h"
#include "internal.h"

Menu::Menu(Screen &screen, Keyboard &keys, const char *t, int sz) : scr(screen), kbd(keys), size(sz), title(t)
{
	list = new String[size];
}

Menu::Menu(Screen &screen, Keyboard &keys, const char *t, const char *items[]) : scr(screen), kbd(keys), title(t)
{
	for (int i=0; items[i]; i++) {
		size ++;
		len ++;
	}
	list = new String[size];
	for (int i=0; i < len; i++)
		list[i] = String(items[i]);
}

void Menu::reset()
{
	len = 0;
	sel = 0;
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
	int hh = h;
	int yy = y;
	scr.clear(x, y, w, h, 0);
	if (title) {
		scr.clear(x, y, w, 1, scr.color(0, 128, 128));
		scr.text(x, y, title, scr.color(255, 255, 0));
		hh --;
		yy ++;
	}
	if (hh <=0)
		return;
	int nr = (sel / hh)*hh;
	for (int pos = nr; pos < len && yy < y + h; pos ++) {
		if (pos == sel)
			scr.clear(x, yy, w, 1, scr.color(0, 0, 255));
		else
			scr.clear(x, yy, w, 1, 0);
		scr.text(x, yy, list[pos].c_str());
		yy ++;
	}
	scr.update();
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
			sel --;
			dirty = true;
			break;
		case KEY_UP:
			sel -= h - 1;
			dirty = true;
			break;
		case KEY_RIGHT:
			sel ++;
			dirty = true;
			break;
		case KEY_DOWN:
			sel += h - 1;
			dirty = true;
			break;
		case KEY_BS:
		case KEY_ESC:
			ret = APP_EXIT;
			break;
		case KEY_MENU:
			ret = APP_MENU;
			break;
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
