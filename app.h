#ifndef __APP_H_INCLUDED
#define __APP_H_INCLUDED
#define APP_EXIT -2
class App {
public:
	virtual int process() {};
	virtual ~App() {};
	virtual void show() {};
};
#endif
