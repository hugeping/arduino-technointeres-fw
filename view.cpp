#include "external.h"
#include "internal.h"

View::View(Screen &screen, Keyboard &keys, const char *t) : scr(screen), kbd(keys), title(t)
{
}

View::~View()
{
	reset();
}

void
View::set(const char *text)
{
	reset();
	append(text);
	cur = start;
}

void
View::down(int nr)
{
	if (visible)
		return;
	for (int i = 0; i<nr; i++) {
		if (cur && cur->next)
			cur = cur->next;
		else
			break;
	}
}

void
View::up(int nr)
{
	for (int i = 0; i<nr; i++) {
		if (cur && cur->prev)
			cur = cur->prev;
		else
			break;
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
			down();
			dirty = true;
			break;
		case KEY_DOWN:
			down(h-1);
			dirty = true;
			break;
		case KEY_LEFT:
			up();
			dirty = true;
			break;
		case KEY_UP:
			up(h-1);
			dirty = true;
			break;
		case KEY_MENU:
			ret = c;
			break;
		case KEY_ENTER:
			ret = c;
			break;
		case KEY_ESC:
			ret = APP_EXIT;
			break;
		case KEY_BS:
			ret = c;
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
View::reset()
{
	while (start) {
		struct line_t *p = start;
		start = start->next;
		free(p);
	}
	start = NULL;
	cur = NULL;
}

void
View::append(const char *text)
{
	int sz = utf8::len(text);
	codepoint_t *buf;
	codepoint_t cp;
	struct line_t *ln = (struct line_t*)malloc(sizeof(struct line_t)
		+ (sz+1)*sizeof(codepoint_t));
	int i;
	if (!ln)
		return;
	ln->next = NULL;
	ln->len = 0;
	if (!start) {
		start = ln;
		ln->prev = NULL;
		cur = ln;
	} else {
		struct line_t *p = start;
		while (p->next) p = p->next;
		p->next = ln;
		ln->prev = p;
	}
	buf = (codepoint_t *)(ln + 1);
	ln->buf = buf;
	for(i = 0; i<sz && *text; i++) {
		text = utf8::to_codepoint(text, &cp);
		buf[i] = cp;
	}
	int xx = 0;
	int yy = 0;
	i = 0;
	int last_i = 0;
	while (i < sz) {
		int ox = xx;
		int oy = yy;
		utf8::fmt_next(buf, &i, sz, w, &xx, &yy);
		if (yy > oy || i == sz) {
			ln->len = i - last_i;
			last_i = i;
			ln->next = NULL;
			if (i < sz) {
				struct line_t *nl = (struct line_t*)malloc(sizeof(struct line_t));
				if (!nl)
					return;
				nl->buf = buf + i;
				nl->prev = ln;
				ln->next = nl;
				ln = nl;
			}
		}
	}
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
	struct line_t *pos = cur;
	for (;pos && hh -- > 0; pos = pos->next) {
		for (int i = 0; i < pos->len && i < w; i ++) {
			scr.clear(xx + i, yy, 1, 1, 0);
			scr.cell(xx + i, yy, pos->buf[i], 0xffff);
		}
		yy ++;
	}
	visible = !pos;
	scr.update();
}
