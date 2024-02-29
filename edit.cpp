#include "edit.h"
static char *text_buf = NULL;
static int text_size = -1;

Edit::Edit(Screen &screen, Keyboard &keys, const char *t, int sz) : scr(screen), kbd(keys), size(sz), title(t)
{
	buf = new codepoint_t[sz+1];
	set("");
}

Edit::Edit(Screen &screen, Keyboard &keys, const char *t, const char *text) : scr(screen), kbd(keys), title(t)
{
	size = utf8::len(text);
	buf = new codepoint_t[size+1];
	set(text);
}
void
Edit::geom(int x, int y, int w, int h)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

const char *
Edit::text()
{
	int sz = 0;
	for (int i = 0; i < len; i ++)
		sz += utf8::from_codepoint(buf[i]);
	if (sz > text_size) {
		if (text_buf)
			free(text_buf);
		text_buf = (char*)malloc(sz + 1);
		text_size = sz;
	}
	char *ptr = text_buf;
	for (int i = 0; i < len; i ++)
		ptr += utf8::from_codepoint(buf[i], ptr);
	*ptr = 0;
	return text_buf;
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
	buf[i] = 0;
	len = i;
}

void
Edit::up()
{
	int skip = w;
	while (cur>0 && skip --) {
		cur --;
		if (buf[cur] == '\n')
			break;
	}
}

void
Edit::down()
{
	cur = utf8::fmt_down(buf, cur, len, w);
}

void
Edit::visible()
{
	if (cur_visible)
		return;
	if (cur < off)
		off = utf8::fmt_up(buf, off, w);
	else
		off = utf8::fmt_down(buf, off, len, w);
	show();
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
		if (c == KEY_ENTER && !oneline) {
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
		switch(c){
		case KEY_DOWN:
			down();
			dirty = true;
			break;
		case KEY_UP:
			up();
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
			ret = APP_EXIT;
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
	if (dirty) {
		show();
		visible();
	}
	return ret;
}

void
Edit::show()
{
	int hh = h;
	int yy = y;
	int xx = x;
	cur_visible = false;
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
		bool atcur = cur == pos;
		cur_visible |= atcur;
		if (utf8::fmt_next(buf, &pos, len, w, &xx, &yy)) {
			scr.clear(ox, oy, 1, 1, (atcur)?0xffff:0);
			scr.cell(ox, oy, cp, (atcur)?0:0xffff);
		}
	}
	scr.update();
}

Edit::~Edit()
{
	delete(buf);
}
