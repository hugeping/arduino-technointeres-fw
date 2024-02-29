#include "view.h"

View::View(Screen &screen, Keyboard &keys, const char *t) : scr(screen), kbd(keys), title(t)
{
	size = 256;
	buf = new codepoint_t[size+1];
}

View::~View()
{
	delete buf;
}

void
View::set(const char *text)
{
	int sz = utf8::len(text);
	if (sz > size) {
		delete(buf);
		buf = new codepoint_t[sz+1];
		size = sz;
	}
	const char *ptr = text;
	codepoint_t cp;
	int i = 0;
	while (*ptr && i < size) {
		ptr = utf8::to_codepoint(ptr, &cp);
		buf[i++] = cp;
	}
	buf[i] = 0;
	len = i;
	off = 0;
}

void
View::down(int nr)
{
	if (visible)
		return;
	for (int i = 0; i<nr; i++)
		off = utf8::fmt_down(buf, off, len, w);
}

void
View::up(int nr)
{
	for (int i = 0; i < nr && off; i ++)
		off = utf8::fmt_up(buf, off, w);
}

int
View::process()
{
	uint8_t c;
	codepoint_t cp;
	const char *sym = NULL;
	int ret = -1;
	bool dirty = false;
	while ((c = kbd.input(&sym)) && ret == -1) {
		switch(c){
		case KEY_RIGHT:
		case KEY_DOWN:
			down();
			dirty = true;
			break;
		case KEY_LEFT:
		case KEY_UP:
			up();
			dirty = true;
			break;
		case KEY_MENU:
		case KEY_ENTER:
		case KEY_ESC:
			ret = APP_EXIT;
			break;
		default:
			break;
		}
	}
	if (dirty)
		show();
	return ret;
}

void
View::show()
{
	int hh = h;
	int yy = 0;
	int xx = 0;
	scr.clear(x, y, w, h, 0);
	if (title) {
		scr.clear(x, y, w, 1, scr.color(0, 128, 128));
		scr.text(x, y, title, scr.color(255, 255, 0));
		hh --;
		yy ++;
	}
	for (int pos = off; pos <= len && yy < h;) {
		codepoint_t cp = buf[pos];
		int ox = xx + x;
		int oy = yy + y;
		visible = pos == len;
		if (utf8::fmt_next(buf, &pos, len, w, &xx, &yy)) {
			scr.clear(ox, oy, 1, 1, 0);
			scr.cell(ox, oy, cp, 0xffff);
		}
	}
	scr.update();
}
