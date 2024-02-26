#include "sensor.h"
static SHT3x sensor;

Sensor::Sensor(Screen &screen, Keyboard &keys) : scr(screen), kbd(keys), t(0), h(0)
{
}

void
Sensor::setup()
{
	sensor.Begin();
}

int
Sensor::process()
{
	int ret = -1;
	uint8_t c;
	sensor.UpdateData();
	int tt = sensor.GetTemperature();
	int hh = sensor.GetRelHumidity();
	if (tt != t || hh != h) {
		t = tt;
		h = hh;
		show();
	}
	while ((c = kbd.input()) && ret == -1) {
		if (c == KEY_ESC)
			ret = -2;
	}
	if (ret == -1)
		delay(100);
	return ret;
}

void
Sensor::show()
{
	char msg[256];
	scr.clear(0, 0, COLS, ROWS, 0);
	sprintf(msg, "Температура: %dC\nВлажность: %d%%", t, h);
	scr.text(0, 0, msg, scr.color(255, 255, 0));
}
