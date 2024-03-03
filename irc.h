#ifndef __IRC_H_INCLUDED
#define __IRC_H_INCLUDED

class Irc : public App {
	View view;
	Edit e_input;
	Edit e_server;
	Edit e_port;
	Edit e_nick;
	Edit e_passwd;
	App *menus[4] = { &e_server, &e_port, &e_nick, &e_passwd };
	Client *cli = NULL;
	WiFiClientSecure &sslclient;
	WiFiClient &client;
	char server[128];
	int port = 6667;
	char pass[64];
	char nick[64];
	char host[64];
	char channel[64];
	char title[64];
	bool connect(const char *host, int port);
	void irc_reply(char *cmd);
	void irc_input(char *cmd);
	void privmsg(char *channel, char *msg);
	void tail();
	void select_menu();
	void connect_irc();
public:
	int process();
	Irc(Screen &scr, Keyboard &kbd, WiFiClient &c, WiFiClientSecure &sc);
	~Irc() { };
	bool select();
};
#endif
