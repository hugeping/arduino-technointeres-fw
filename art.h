class Art : public App {
	Preferences prefs;
	Screen &scr;
	Keyboard &kbd;
	WiFiClientSecure &client;
	int start = 0;
	void http_request(const char *req);
	bool request();
	void display_block(int nr, uint8_t *buf);
	void display(const char *title, uint8_t *buf);
public:
	int process();
	Art(Screen &scr, Keyboard &kbd, WiFiClientSecure &c);
	bool select();
};
