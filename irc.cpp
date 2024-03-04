#include "external.h"
#include "internal.h"

Irc::Irc(Screen &scr, Keyboard &kbd, WiFiClient &c, WiFiClientSecure &sc) :
	view(scr, kbd, "Irc"), client(c), sslclient(sc),
	e_input(scr, kbd, NULL, 250),
	e_server(scr, kbd, "Server", 64),
	e_port(scr, kbd, "Port", 64),
	e_nick(scr, kbd, "Nick", 64),
	e_passwd(scr, kbd, "Password", 64)
{
	last_usr[0] = 0;
	e_input.y = ROWS-1;
	e_input.h = 1;
	e_input.oneline = true;
	e_server.oneline = e_port.oneline =
		e_nick.oneline = e_passwd.oneline = true;
	e_server.active = e_port.active = e_nick.active =
		e_passwd.active = false;
	e_server.h = e_port.h = e_nick.h = e_passwd.h = 2;
	e_server.y = 1;
	e_port.y = 3;
	e_nick.y = 5;
	e_passwd.y = 7;
	view.h = ROWS - 1;
	pass[0] = 0;
	sprintf(server, "irc.oftc.net");
	sprintf(nick, "esp-user");
	sprintf(host, "esp32-c3");
	channel[0] = 0;
	sprintf(title, "IRC");
	view.title = title;
}

static char*
skip(char *s, char c)
{
        while(*s != c && *s != '\0')
                s++;
        if(*s != '\0')
                *s++ = '\0';
        return s;
}
static void
trim(char *s)
{
        char *e;
        for (e = s + strlen(s); e > s && isspace((unsigned char)*(e - 1)); e--);
        *e = '\0';
}

static char *
eat(char *s, int (*p)(int), int r)
{
        while(*s != '\0' && p((unsigned char)*s) == r)
                s++;
        return s;
}

void
Irc::privmsg(char *channel, char *msg)
{
        char fmt[256];
        if(channel[0] == '\0') {
                view.append("No channel to send to");
                return;
        }
        if (!strcmp(channel, this->channel))
        	sprintf(fmt, "<%s> %s", nick, msg);
        else
        	sprintf(fmt, "%s: <%s> %s", channel, nick, msg);
        view.append(fmt);
        sprintf(fmt, "PRIVMSG %s :%s", channel, msg);
        cli->println(fmt);
}


void
Irc::irc_input(char *s)
{
	char fmt[128];
	char c, *p;
	if (!s[0])
		return;
	skip(s, '\n');
	if (s[0] == '/') { /* as is */
		cli->println(s + 1);
		return;
	}
	if (s[0] != ':') {
		privmsg(channel, s);
		return;
	}
	c = *++s;
	if (c && s[1] == ' ') {
		p = s + 2;
		p += strspn(p, " ");
		switch(c) {
		case 'j':
			sprintf(fmt, "JOIN %s", p);
			cli->println(fmt);
			strcpy(channel, p);
			return;
		case 'l':
			s = eat(p, isspace, 1);
			p = eat(s, isspace, 0);
			if (!*s)
				s = channel;
			if (*p)
				*p++ = 0;
			if (!*p)
				p = "Bye!";
			sprintf(fmt, "PART %s :%s", s, p);
			cli->println(fmt);
			return;
		case 'm':
			s = eat(p, isspace, 1);
			p = eat(s, isspace, 0);
			if(*p)
				*p++ = 0;
			privmsg(s, p);
			return;
		case 's':
			strcpy(channel, p);
			sprintf(fmt, "Channel is '%s'", p);
			view.append(fmt);
			return;
		default:
			view.append(":j <chan>\n:s <chan>\n:m <nick> <msg>\n:l [msg]");
			return;
		}
	}
	privmsg(channel, s);
	return;
}

void
Irc::irc_reply(char *cmd)
{
	char fmt[1024];
	char *usr = host;
	char *par, *txt;
	if (!cmd[strspn(cmd, " \t\n")])
		return;
	if (cmd[0] == ':') {
		usr = cmd + 1;
		cmd = skip(usr, ' ');
		if (!cmd[0])
			return;
		skip(usr, '!');
	}
	skip(cmd, '\r');
	par = skip(cmd, ' ');
	txt = skip(par, ':');
	trim(par);
	if (!strcmp("PING", cmd))
		return;
	if (!strcmp("PRIVMSG", cmd)) {
		sprintf(fmt, "<%s> %s", usr, txt);
		view.append(fmt);
	} else if (!strcmp("PING", cmd)) {
		sprintf(fmt, "PONG ", txt);
		cli->println(fmt);
	} else {
		if (strcmp(usr, last_usr)) {
			sprintf(fmt, "%s: ", usr);
			strcpy(last_usr, usr);
		} else
			fmt[0] = 0;
		if (*par)
			sprintf(fmt + strlen(fmt), "%s(%s): ", cmd, par);
		else if (*cmd)
			sprintf(fmt + strlen(fmt), "%s: ", cmd);
		strcat(fmt, txt);
//		sprintf(fmt, "%s: >< %s (%s): %s", usr, cmd, par, txt);
		view.append(fmt);
		if (!strcmp("NICK", cmd) && !strcmp(usr, nick))
			strcpy(nick, txt);
	}
}

void
Irc::tail()
{
	view.trim_head(view.h*30);
	if (app() == &e_input) {
		view.tail();
		view.show();
	}
}

int
Irc::process()
{
	char fmt[1024];
	bool dirty = false;
	bool dotail = view.visible;
	if (cli && cli->available()) {
		String line = cli->readStringUntil('\n');
		strcpy(fmt, line.c_str());
		irc_reply(fmt);
		if (dotail)
			tail();
	}
	int m = app()->process();
	if (app() == &e_input) {
		if (m == KEY_UP) {
			view.up(view.h-1);
			view.show();
		} else if (m == KEY_DOWN) {
			view.down(view.h-1);
			view.show();
		} else if (m == KEY_MENU) {
			select_menu();
		} else if (m == KEY_ENTER) {
			strcpy(fmt, e_input.text());
			e_input.set("");
			e_input.show();
			irc_input(fmt);
			sprintf(title, "%s %s", server, channel);
			tail();
		} else if (m == APP_EXIT) {
			if (cli)
				cli->stop();
			cli = NULL;
			return m;
		}
	} else { // menu
		if (m == APP_EXIT) {
			set(&e_input);
			tail();
			return 0;
		}
		for (int i = 0; i < menus_nr; i++) {
			if (menus[i] == app()) {
				if (m == KEY_UP || m == KEY_DOWN) {
					app()->active = false;
					app()->show();
					i += ((m == KEY_UP)? -1:1);
					i = (i<0)?menus_nr-1:i;
					i = (i>=menus_nr)?0:i;
					menus[i]->active = true;
					set(menus[i]);
				} else if (m == KEY_ENTER) {
					strcpy(server, e_server.text());
					port = atoi(e_port.text());
					if (port <= 0)
						port = 6667;
					strcpy(nick, e_nick.text());
					strcpy(pass, e_passwd.text());
					prefs.begin("IRC", false);
					prefs.putString("server", e_server.text());
					prefs.putInt("port", port);
					prefs.putString("nick", e_nick.text());
					prefs.putString("passwd", e_passwd.text());
					prefs.end();
					connect_irc();
					return 0;
				}
			}
		}
	}
	return m;
}

bool
Irc::connect(const char *host, int port)
{
	cli = &client;
	if (port == 6697 || port == 9999)
		cli = &sslclient;
	view.show();
	if (!cli->connect(host, port)) {
		view.append("Can't connect...");
		return false;
	}
	return true;
}

void
Irc::connect_irc()
{
	if (cli)
		cli->stop();
	cli = NULL;
	set(&e_input);
	view.reset();
	if (connect(server, port)) {
		sprintf(title, "%s", server);
		char fmt[256];
		if (pass[0]) {
			sprintf(fmt, "PASS %s", pass);
			cli->println(fmt);
		}
		sprintf(fmt, "NICK %s", nick);
		cli->println(fmt);
		sprintf(fmt, "USER %s localhost %s : %s", nick, host, nick);
		cli->println(fmt);
	}
	view.show();
}

void
Irc::select_menu()
{
	e_server.set(server);
	e_server.show();
	e_port.set(String(port).c_str());
	e_port.show();
	e_nick.set(nick);
	e_nick.show();
	e_passwd.set(pass);
	e_passwd.show();
	e_server.active = true;
	set(&e_server);
}

bool
Irc::select()
{
	reset();
	view.show();
	prefs.begin("IRC", true);
	String s = prefs.getString("server");
	if (s != "")
		strcpy(server, s.c_str());
	int p = prefs.getInt("port");
	if (p > 0)
		port = p;
	s = prefs.getString("nick");
	if (s != "")
		strcpy(nick, s.c_str());
	s = prefs.getString("passwd");
	if (s != "")
		strcpy(pass, s.c_str());
	prefs.end();
	select_menu();
	return true;
}
