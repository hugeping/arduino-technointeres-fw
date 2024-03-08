class Art : public App {
	Preferences prefs;
	Screen &scr;
	Keyboard &kbd;
	Edit m_page;
	WiFiClientSecure &client;
	int start = 0;
	int total = 0;
	void http_request(const char *req);
	bool request();
	void display_block(int nr, uint8_t *buf);
	void display(const char *title, uint8_t *buf);
public:
	int process();
	void setup();
	Art(Screen &scr, Keyboard &kbd, WiFiClientSecure &c);
	bool select();
};
