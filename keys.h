#ifndef __KEYBOARD_H_INCLUDED
#define __KEYBOARD_H_INCLUDED
#include <ADS1115_lite.h>

#define RU 3
#define EN 10

#define KEY_UP 10
#define KEY_DOWN 11
#define KEY_LEFT 21
#define KEY_RIGHT 22
#define KEY_LAYOUT 28
#define KEY_ENTER 19
#define KEY_MENU 26
#define KEY_BS 18
#define KEY_ESC 15

struct keysym_t {
	uint8_t code;
	struct layout {
		const char *sym;
		const char *shift_sym;
		const char *extra_sym;
	} sym[2];
};

static keysym_t row0[] = {
	{ 0, { {"g", "G", ")" }, { "п" ,"П", ")"} } },
	{ 1, { {"r", "R", "5" }, { "к" ,"К", "5"} } },
	{ 255, { {}, {} } },
	{ 3, { {"e", "E", "4" }, { "у", "У", "4"} } },
	{ 4, { {"f", "F", "(" }, { "а", "А", "("} } },
	{ 5, { {"x", "X", "}" }, { "ч", "Ч", "}"} } },
	{ 6, { {"z", "Z", "{" }, { "я", "Я", "{"} } },
	{ 7, { {"d", "D", "@" }, { "в", "В", "@"} } },
	{ 8, { {"s", "S", "/" }, { "ы", "Ы", "/"} } },
	{ 9, { {"a", "A", "\\" }, { "ф", "Ф", "\\" } } },
	{ 10, { {}, {} } }, // up
	{ 11, { {}, {} } }, // down
	{ 12, { { NULL, NULL, "1" }, { "ё", "Ё", "1" } } },
	{ 13, { {"q", "Q", "2" }, { "й", "Й", "2" } } },
	{ 14, { {"w", "W", "3" }, { "ц", "Ц", "3" } } },
	{ 15, { {}, {} } }, // esc
};

static keysym_t row1[] = {
	{ 16, { {}, {} } }, // extra
	{ 17, { {}, {} } }, // shift
};

static keysym_t row2[] = {
	{ 18, { {}, {} } }, // bs
	{ 19, { {}, {} } }, // enter
	{ 255, { {}, {} } },
	{ 20, { { NULL, NULL, "]" }, {"ъ", "Ъ"} } },
	{ 21, { {}, {} } }, // left
	{ 22, { {}, {} } }, // right
	{ 23, { { NULL, NULL, "[" }, { "х", "Х" } } },
	{ 24, { { "p", "P", "-" }, { "з", "З", "-" } } },
	{ 25, { { "=", "+" }, { "=", "+" } } },
	{ 26, { { }, { } } }, // menu
	{ 27, { { "o", "O", "0" }, { "щ", "Щ", "0" } } },
	{ 28, { {}, {} } }, // layout
	{ 29, { { "i", "I", "9" }, { "ш", "Ш", "9" } } },
	{ 30, { { "u", "U", "8" }, { "г", "Г", "8" } } },
	{ 31, { { "y", "Y", "7" }, { "н", "Н", "7" } } },
};

static keysym_t row3[] = {
	{ 32, { { "j", "J", "*" }, { "о", "О", "*" } } },
	{ 33, { { "k", "K", "\"" }, { "л", "Л", "\"" } } },
	{ 255, { {}, {} } },
	{ 35, { { "l", "L", ":" }, { "д", "Д", ":" } } },
	{ 36, { { NULL, NULL, ";" }, { "ж", "Ж", ";" } } },
	{ 37, { { NULL, NULL, "'" }, { "э", "Э", "'" } } },
	{ 38, { { NULL, NULL, "." }, { "ю", "Ю", "." } } },
	{ 39, { { NULL, NULL, "," }, { "б", "Б", "," } } },
	{ 40, { { "m", "M", "!" }, { "ь", "Ь", "!" } } },
	{ 41, { { "n", "N", "?" }, { "т", "Т", "?" } } },
	{ 42, { { "b", "B", "&" }, { "и", "И", "&" } } },
	{ 43, { { " " }, { " " } } },
	{ 44, { { "v", "V", ">" }, { "м", "М", ">" } } },
	{ 45, { { "h", "H", "%" }, { "р", "Р", "%" } } },
	{ 46, { { "c", "C", "<" }, { "с", "С", "<" } } },
	{ 255, { {}, {} } },
};

class Keyboard
{
	ADS1115_lite ads;
	uint8_t layout;
	struct {
		int start;
		int end;
		struct {
			uint8_t code;
			const char *sym;
		} keys[4];
	} inp_fifo;
	void ads_config();
	int16_t ads_read(int mux);
	void poll_keys(int out[4]);
	void layout_toggle();
public:
	Keyboard();
	void setup();
	uint8_t input(const char **sym = NULL);
	void inp_add(uint8_t code, const char *sym);
	void poll();
};
/*
'g', 'r', ?, 'e', f, x, z, d, s, a, up, down, q, w, esc
extra, shift,
bs, enter, ??, ], <-, =>, [, p, +, menu, o, lay, i. u, y,
j, k, ??, ;, ', ., ,, !, n, b, " ", v, h, c,
*/
#endif
