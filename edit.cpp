#include "edit.h"

Edit::Edit(Screen &screen, Keyboard &keys, const char *t, int sz) : scr(screen), kbd(keys), size(sz), title(t)
{
	len = 0;
	buf = new codepoint_t[sz];
}

Edit::Edit(Screen &screen, Keyboard &keys, const char *t, const char *text) : scr(screen), kbd(keys), title(t)
{
	size = utf8::len(text);
	buf = new codepoint_t[size];
	set(text);
}

void
Edit::set(const char *text)
{
	const char *ptr = text;
	codepoint_t cp;
	int i = 0;
	do {
		ptr = utf8::to_codepoint(ptr, &cp);
		buf[i++] = cp;
	} while (*ptr && i < size);
	len = i;
}

int
Edit::process()
{
	return -1;
}

void
Edit::show()
{
}

Edit::~Edit()
{
	delete(buf);
}
