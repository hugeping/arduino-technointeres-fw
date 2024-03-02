#ifndef __WIFILIST_H_INCLUDED
#define __WIFILIST_H_INCLUDED

class Wifilist : public App {
	Edit e_pass;
	Menu m_cancel;
	Menu m_wifi;
	View v_info;
	Preferences prefs;
public:

	Wifilist(Screen &scr, Keyboard &kbd);
	void setup();
	bool select();
	int process();
};
#endif
