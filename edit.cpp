#include "edit.h"

Edit::Edit(Screen &screen, Keyboard &keys, const char *t, int sz) : scr(screen), kbd(keys),
	size(sz), title(t), cur(0), x(0), y(0), w(COLS), h(ROWS), off(0), len(0)
{
	buf = new codepoint_t[sz+1];
}

Edit::Edit(Screen &screen, Keyboard &keys, const char *t, const char *text) : scr(screen), kbd(keys),
	title(t), cur(0), x(0), y(0), w(COLS), h(ROWS), off(0)
{
	size = utf8::len(text);
	buf = new codepoint_t[size+1];
	set(text);
}

void
Edit::set(const char *text)
{
	const char *ptr = text;
	codepoint_t cp;
	int i = 0;
	while (*ptr && i < size) {
		ptr = utf8::to_codepoint(ptr, &cp);
		buf[i++] = cp;
	}
	len = i;
}

int
Edit::process()
{
	uint8_t c;
	codepoint_t cp;
	const char *sym = NULL;
	int ret = -1;
	bool dirty = false;
	while ((c = kbd.input(&sym)) && ret == -1) {
		if (c == KEY_ENTER) {
			sym = "\n";
		}
		if (sym && len < size) {
			memmove(&buf[cur+1], &buf[cur], sizeof(codepoint_t)*(len-cur+1));
			utf8::to_codepoint(sym, &cp);
			buf[cur] = cp;
			cur ++;
			len ++;
			dirty = true;
			c = -1;
			ret = 0;
		}
		int skip = w;
		switch(c){
		case KEY_DOWN:
			while (cur < len && skip --) {
				if (buf[cur++] == '\n')
					break;
			}
			dirty = true;
			break;
		case KEY_UP:
			while (cur>0 && skip --) {
				cur --;
				if (buf[cur] == '\n')
					break;
			}
			dirty = true;
			break;
		case KEY_LEFT:
			cur --;
			dirty = true;
			break;
		case KEY_RIGHT:
			cur ++;
			dirty = true;
			break;
		case KEY_BS:
			if (cur > 0 && len >0) {
				memmove(&buf[cur-1], &buf[cur], sizeof(codepoint_t)*(len-cur+1));
				cur --;
				len --;
				dirty = true;
			}
			break;
		case KEY_ESC:
			ret = -2;
			break;
		case KEY_MENU:
		case KEY_ENTER:
			ret = c;
			break;
		default:
			break;
		}
	}
	cur = max(0, cur);
	cur = min(cur, len);
	// scr.text(0, 16, String(sel).c_str());
	if (dirty)
		show();
	return ret;
}

void
Edit::show()
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
		scr.clear(xx, yy, 1, 1, (cur == pos)?0xffff:0);
		if (cp == '\n') {
			yy ++;
			xx = x;
		} else {
			scr.cell(xx, yy, cp, (cur == pos)?0:0xffff);
			xx ++;
			if (xx >= x + w) {
				xx = x;
				yy ++;
			}
		}
	}
	while (yy < y + h) {
		scr.clear(xx, yy, 1, 1, 0);
		scr.cell(xx, yy, 0, 0);
		xx ++;
		if (xx >= x + w) {
			xx = x;
			yy ++;
		}
	}
	scr.update();
}

Edit::~Edit()
{
	delete(buf);
}
