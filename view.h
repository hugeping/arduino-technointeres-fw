#ifndef __VIEW_H_INCLUDED
#define __VIEW_H_INCLUDED

class View : public App {
	codepoint_t *buf;
	Screen &scr;
	Keyboard &kbd;
	boolean visible = false;
public:
	const char *title;

	int x = 0;
	int y = 0;
	int w = COLS;
	int h = ROWS;
	int off = 0;
	int size = 0;
	int len = 0;

	View(Screen &scr, Keyboard &kbd, const char *title);

	~View();
	void set(const char *text);
	void up(int nr = 1);
	void down(int nr = 1);
	int process();
	void show();
};
#endif
