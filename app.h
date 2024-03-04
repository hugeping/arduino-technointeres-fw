#ifndef __APP_H_INCLUDED
#define __APP_H_INCLUDED

#define APP_EXIT -2
#define APP_MENU -3
#include "screen.h"
class App {
	App *apps[16];
	int app_nr = 0;
public:
	bool active = true;
	virtual void setup() {};
	void reset() { app_nr = 0; };
	App() {};
	App(App *app) {
		push(app);
	};
	virtual int process() {
		if (!app_nr)
			return APP_EXIT;
		return app()->process();
	};
	virtual bool select() { show(); return true; };
	virtual ~App() {};
	virtual void show() {
		if (!app_nr)
			return;
		app()->show();
	};
	void push(App *a) {
		if (!a->select())
			return;
		apps[app_nr++] = a;
	};
	void set(App *a) {
		if (app_nr == 0)
			app_nr ++;
		apps[app_nr-1] = a;
		a->select();
	};
	App *pop() {
		if (app_nr<2)
			return NULL;
		//scr.clear();
		App *a = apps[--app_nr];
		apps[app_nr-1]->show();
		return a;
	};
	App *app() {
		return apps[app_nr -1];
	};
};
#endif
