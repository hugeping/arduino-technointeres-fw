#ifndef __GEMINI_H_INCLUDED
#define __GEMINI_H_INCLUDED

class Gemini : public App {
	View view;
	WiFiClientSecure &client;
public:
	Gemini(Screen &scr, Keyboard &kbd, WiFiClientSecure &c);
	bool select();
};
#endif
