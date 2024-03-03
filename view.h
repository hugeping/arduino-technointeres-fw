#ifndef __VIEW_H_INCLUDED
#define __VIEW_H_INCLUDED


class View : public App {
	struct line_t;

	struct line_t {
		struct line_t *next;
		struct line_t *prev;
		codepoint_t *buf;
		int len;
	};

	struct line_t *start = NULL;
	struct line_t *cur = NULL;
	Screen &scr;
	Keyboard &kbd;
	boolean visible = false;
public:
	const char *title;

	int x = 0;
	int y = 0;
	int w = COLS;
	int h = ROWS;

	View(Screen &scr, Keyboard &kbd, const char *title);

	~View();
	void append(const char *text);
	void set(const char *text);
	void reset();
	void up(int nr = 1);
	void down(int nr = 1);
	int process();
	void show();
};
#endif
