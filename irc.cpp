#include "external.h"
#include "internal.h"

Irc::Irc(Screen &scr, Keyboard &kbd, WiFiClient &c, WiFiClientSecure &sc) :
	view(scr, kbd, "Irc"), client(c), sslclient(sc),
	e_input(scr, kbd, NULL, 256)
{
	e_input.y = ROWS-1;
	e_input.h = 1;
	e_input.oneline = true;
	view.h = ROWS - 1;
	pass[0] = 0;
	sprintf(server, "irc.oftc.net");
	sprintf(nick, "esp-%02x", random()&0xff);
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
		sprintf(fmt, "%s: >< %s (%s): %s", usr, cmd, par, txt);
		view.append(fmt);
		if (!strcmp("NICK", cmd) && !strcmp(usr, nick))
			strcpy(nick, txt);
	}
}

void
Irc::tail()
{
	view.trim_head(view.h*2);
	view.tail();
	view.show();
}
int
Irc::process()
{
	char fmt[1024];
	bool dirty = false;
	if (cli && cli->available()) {
		String line = cli->readStringUntil('\n');
		strcpy(fmt, line.c_str());
		irc_reply(fmt);
		tail();
	}
	int m = app()->process();
	if (app() == &e_input) {
		if (m == KEY_UP) {
			view.up();
			view.show();
		} else if (m == KEY_DOWN) {
			view.down();
			view.show();
		} else if (m == KEY_ENTER) {
			strcpy(fmt, e_input.text());
			e_input.set("");
			e_input.show();
			irc_input(fmt);
			sprintf(title, "%s %s", server, channel);
			tail();
		}
	}
	return m;
}

bool
Irc::connect(const char *host, int port)
{
	cli = &client;
	view.show();
	if (!cli->connect(host, port)) {
		view.append("Can't connect...");
		return false;
	}
	view.append("Connected!");
	return true;
}

bool
Irc::select()
{
	Serial.println("Select irc");
	view.show();
	set(&e_input);
	if (connect(server, port)) {
		sprintf(title, "%s", server);
		char fmt[256];
		if (pass[0]) {
			sprintf(fmt, "PASS %s");
			cli->println(fmt);
		}
		sprintf(fmt, "NICK %s", nick);
		cli->println(fmt);
		sprintf(fmt, "USER %s localhost %s : %s", nick, host, nick);
		cli->println(fmt);
	}
	view.show();
	return true;
}
