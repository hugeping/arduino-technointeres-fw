#ifndef __APP_H_INCLUDED
#define __APP_H_INCLUDED

#define APP_EXIT -2
class App {
	App *apps[16];
	int app_nr = 0;
public:
	virtual int process() {};
	virtual bool select() { show(); return true; };
	virtual ~App() {};
	virtual void show() {};
};
#endif
