#include "view.h"

View::View(Screen &screen, Keyboard &keys, const char *t) : scr(screen), kbd(keys), title(t), x(0), y(0), w(COLS), h(ROWS), off(0), len(0), size(0)
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
	}
	const char *ptr = text;
	codepoint_t cp;
	int i = 0;
	while (*ptr && i < size) {
		ptr = utf8::to_codepoint(ptr, &cp);
		buf[i++] = cp;
	}
	len = i;
}

void
View::down()
{
	int skip = w;
	while (off < len && skip --) {
		if (buf[off++] == '\n')
			break;
	}
}

void
View::up()
{
	int skip = w;
	boolean once = false;
	while (off > 0 && skip --) {
		off --;
		if (buf[off] == '\n') {
			if (once)
				break;
			once = true;
		}
	}
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
	int yy = y;
	int xx = x;
	scr.clear(x, y, w, h, 0);
	if (title) {
		scr.clear(x, y, w, 1, scr.color(0, 128, 128));
		scr.text(x, y, title, scr.color(255, 255, 0));
		hh --;
		yy ++;
	}
	for (int pos = off; pos <= len && yy < y + h; pos ++) {
		codepoint_t cp = buf[pos];
		scr.clear(xx, yy, 1, 1, 0);
		if (cp == '\n') {
			yy ++;
			xx = x;
		} else {
			scr.cell(xx, yy, cp, 0xffff);
			xx ++;
			if (xx >= x + w) {
				xx = x;
				yy ++;
			}
		}
	}
	scr.update();
}
