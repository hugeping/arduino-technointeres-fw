#include "keys.h"

static keysym_t *keymap[] =  { row0, row1, row2, row3 };

void
Keyboard::ads_config()
{
	ads.setGain(ADS1115_REG_CONFIG_PGA_4_096V); // GAIN_ONE and resolution to ± 4.096V
	ads.setSampleRate(ADS1115_REG_CONFIG_DR_250SPS); // Samples per sec//8 16 32 64 128 250 475 860
}

int16_t
Keyboard::ads_read(int mux)
{
	switch (mux) {
	case 0:
		ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_0);  // Single mode input on AIN0 (AIN0 - GND)
		break;
	case 1:
		ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_1);  // Single mode input on AIN1 (AIN0 - GND)
		break;
	case 2:
		ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_2);  // Single mode input on AIN2 (AIN0 - GND)
		break;
	case 3:
		ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_3);  // Single mode input on AIN3 (AIN0 - GND)
		break;
	}
	ads.triggerConversion();  // TriggeTFT_RED mannually
	return ads.getConversion();  // returns int16_t value
}

Keyboard::Keyboard()
{
	ads = ADS1115_lite(ADS1115_DEFAULT_ADDRESS);  // 0x48 addr pin connected to GND
	inp_fifo = { 0, 0, { }};
	layout = 0;
}

void
Keyboard::setup()
{
	pinMode(RU, OUTPUT);
	pinMode(EN, OUTPUT);
	ads_config();
	layout_toggle();
}

struct {
	int start;
	int end;
	struct {
		uint8_t code;
		const char *sym;
	} keys[4];
} inp_fifo = { 0, 0, { }};

void
Keyboard::inp_add(uint8_t code, const char *sym)
{
	int end = inp_fifo.end;
	inp_fifo.keys[end].code = code;
	inp_fifo.keys[end].sym = sym;
	end = (end + 1)%4;
	inp_fifo.end = end;
}

uint8_t
Keyboard::input(const char **sym)
{
	uint8_t c;
	int start = inp_fifo.start;
	c = inp_fifo.keys[start].code;
	if (c) {
		if (sym)
			*sym = inp_fifo.keys[start].sym;
		inp_fifo.keys[start].code = 0;
		start = (start + 1) % 4;
		inp_fifo.start = start;
	}
	return c;
}

void
Keyboard::poll_keys(int out[4])
{
	int16_t raw[4];
	static float btns[4] = { 16, 2, 15, 16 };
	for (int i=0; i < 4; i ++)
		raw[i] = ads_read(i);
	float res = max(max(raw[0], raw[1]), max(raw[2], raw[3]));
	for (int i=0; i < 4; i ++) {
		out[i] = round((float)raw[i] * btns[i] / res);
		if (out[i] == btns[i])
			out[i] = -1;
	}
  //tft.fillRect(0, 220, 240, 10, color(0, 0, 255));
//  char msg[255];
//  for (int i = 0; i < 4; i ++) {
//    sprintf(msg, "%d", out[i]);
//    screen_text(0 + i*3, ROWS-3, msg);
//  }
}

void
Keyboard::layout_toggle()
{
	layout = layout ^ 1;
	if (layout) {
		analogWrite(RU, 128);
		analogWrite(EN, 0);
	} else {
		analogWrite(RU, 0);
		analogWrite(EN, 128);
	}
}

void
Keyboard::poll(void)
{
	int i;
	struct keysym_t *sym;
//	char msg[256];

	int rows[4];
	int rows2[4];

	static int last_rows[4] = { -1, -1, -1, -1 };

	uint8_t press[4] = { 255, 255, 255, 255 };
	uint8_t release[4] = { 255, 255, 255, 255 };

	boolean fuzzy;
	do {
		fuzzy = false;
		poll_keys(rows);
		poll_keys(rows2);
		for (i = 0; i < 4; i++)
			if (rows[i] != rows2[i]) {
				fuzzy = true;
				break;
			}
	} while (fuzzy);

	int extra_sym = (rows[1] == 0);
	int shift_sym = (rows[1] == 1);

	for (i = 0; i < 4; i ++) {
		if (rows[i] == last_rows[i])
			continue;
		if (rows[i] == -1) {
			release[i] = last_rows[i];
		} else if (last_rows[i] == -1) {
			press[i] = rows[i];
		} else {
			release[i] = last_rows[i];
			press[i] = rows[i];
		}
		last_rows[i] = rows[i];
	}

	for (i = 0; i < 4; i ++) {
		uint8_t key = release[i];
		if (key != 255) {
			sym = &keymap[i][key];
			if (sym->code == KEY_LAYOUT)
				layout_toggle();
		}
		key = press[i];
		if (key != 255) {
			const char *s;
			sym = &keymap[i][key];
			if (extra_sym)
				s = sym->sym[layout].extra_sym;
			else if (shift_sym)
				s = sym->sym[layout].shift_sym;
			else
				s = sym->sym[layout].sym;
			inp_add(sym->code, s);
//			if (s) {
//        sprintf(msg, "%s", s);
//		tft.fillRect(0, 230, 240, 10, color(0, 0, 255));
//        screen_text(0 + i*3, ROWS-2, msg);
//			}
		}
	}
}
