#ifndef __WIFILIST_H_INCLUDED
#define __WIFILIST_H_INCLUDED

class Wifilist : public App {
	Edit wifi_pass;
	Menu wifi_cancel;
	Menu wifi_menu;
	View info;
	App *app;
public:

	Wifilist(Screen &scr, Keyboard &kbd);
	bool select();
	int process();
};
#endif
