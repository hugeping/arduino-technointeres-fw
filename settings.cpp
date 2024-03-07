#include "external.h"
#include "internal.h"

Settings::Settings(Screen &screen, Keyboard &keys) : scr(screen), kbd(keys),
	m_main(screen, keys, "Settings", 16)
{
	m_main.append("Brightness");
}

void
Settings::show_bright()
{
	char fmt[256];
	if (mode == brightness)
		sprintf(fmt, "Brightness [%d]", set_br);
	else
		sprintf(fmt, "Brightness");
	m_main.set(0, fmt);
	m_main.show();
	scr.update();
}

void
Settings::setup()
{
	pinMode(7, OUTPUT); /* brightness */
	prefs.begin("settings", true);
	set_br = prefs.getInt("brightness", -1);
	if (set_br < 0)
		set_br = 128;
	prefs.end();
	set_br = min(set_br, 255);
	set_br = max(set_br, 1);
	analogWrite(7, set_br);
}

int
Settings::process()
{
	char fmt[256];
	if (mode == brightness) {
		uint8_t c;
		while ((c = kbd.input())) {
			if (c == KEY_ESC || c == KEY_BS || c == KEY_ENTER) {
				mode = none;
				show_bright();
				prefs.begin("settings", false);
				prefs.putInt("brightness", set_br);
				prefs.end();
				return 0;
			}
			if (c == KEY_DOWN || c == KEY_LEFT)
				set_br -= (c == KEY_DOWN)?10:1;
			else if (c == KEY_UP || c == KEY_RIGHT)
				set_br += (c == KEY_UP)?10:1;
			set_br = max(1, set_br);
			set_br = min(255, set_br);
			analogWrite(7, set_br);
			show_bright();
		}
		return 0;
	}
	int m = app()->process();
	if (app() == &m_main) {
		if (m == APP_EXIT)
			return m;
		switch(m) {
		case 0:
			mode = brightness;
			show_bright();
			break;
		}
	}
	return 0;
}

bool
Settings::select()
{
	reset();
	set(&m_main);
	return true;
}
