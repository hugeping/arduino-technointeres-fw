class Settings : public App {
	enum settings_mode {
		none = 0,
		brightness,
	};
	int set_br = 128;
	Preferences prefs;
	Screen &scr;
	Keyboard &kbd;
	Menu m_main;
	settings_mode mode = none;
	void show_bright();
public:
	Settings(Screen &scr, Keyboard &kbd);
	int process();
	void setup();
	bool select();
};
