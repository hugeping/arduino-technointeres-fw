#include "external.h"
#include "internal.h"

Gemini::Gemini(Screen &scr, Keyboard &kbd, WiFiClientSecure &c) :
	view(scr, kbd, "Gemini"), client(c),
	v_status(scr, kbd, "Status"),
	m_links(scr, kbd, "Links", MAX_LINKS)
{
}

bool
Gemini::req(const char* req)
{
	String ans;
	const char *str;
	client.println(req);
	ans = client.readStringUntil('\n');
	str = ans.c_str();
	int len = ans.length();
	if (len<3 || str[2] != ' ') {
		status[0] = 0;
		meta[0] = 0;
		return false;
	}
	status[0] = str[0]; status[1] = str[1];
	status[2] = 0;
	str += 3;
	len -= 3;
	if (len > 1024)
		memcpy(meta, str, 1024);
	else
		strcpy(meta, str);
	meta[strcspn(meta, "\r")] = 0;
	meta[1024] = 0;
	return true;
}

void
Gemini::body()
{
	String out; // = String(last_url)+"\n";
	m_links.reset();
	links_nr = 0;
	while (client.connected() || client.available()) {
		String line = client.readStringUntil('\n');
		line.replace("\r", "");
		if (line.startsWith("=>")) {
			int s = 2;
			while (line[s] == ' ') s++;
			String link;
			int idx = line.indexOf(' ', s);
			if (idx > 0) {
				link = line.substring(s, idx);
				while (line[idx] == ' ') idx ++;
				line.remove(s, idx - s);
			} else {
				link = line.substring(s);
			}
			if (link.startsWith("gemini://") ||
				link.startsWith("/") ||
				link.startsWith("./")) {
				links[links_nr++] = link;
				m_links.append(line.c_str() + s);
			}
		}
		out += line + "\n";
	}
	view.set(out.c_str());
}

static void
normpath(char *path)
{
	const char *src = path;
	char *dst = path;

	while (*src) {
		if (src[0] == '.') {
			if (src[1] == '/') {
				src += 2;
			} else if(src[1] == '.' && src[2] == '/') {
				if (dst > path)
					dst --;
				while (dst > path && *(--dst) != '/');
				src += 3;
			}
		}
		do {
			*dst++ = *src;
		} while(*src && *(src++) != '/');
		src += strspn(src, "/");
	}
}

bool
Gemini::reqURI(const char *uri, bool hist)
{
	char url[1025];
	const char *ptr = uri;
	if (last_url && hist) {
		history[(hist_pos++)%hist_max] = String(last_url);
		hist_size ++;
	}
	if (!strncmp(uri, "gemini://", 9)) {
		free(server);
		free(last_url);
		last_url = strdup(uri);
		ptr += 9;
		server = strdup(ptr);
		server[strcspn(server, "/")] = 0;
		view.title = server;
		if (!client.connect(server, 1965))
			return false;
	} else if ((uri[0] == '/' || uri[0] == '.') && server) {
		if (!client.connect(server, 1965))
			return false;
		if (uri[0] == '.')
			sprintf(url, "%s/%s", last_url, uri);
		else
			sprintf(url, "gemini://%s%s", server, uri);
		free(last_url);
		last_url = strdup(url);
	} else
		return false;

	normpath(last_url + 9);
	if (!req(last_url) || status[0] != '2') {
		char fmt[256];
		sprintf(fmt, "%s\nStatus:%s", last_url, status);
		view.set(fmt);
		view.show();
		return false;
	}
	return true;
}

int
Gemini::process()
{
	int m = app()->process();
	if (app() == &view && m == KEY_MENU) {
		push(&m_links);
	} else if (app() == &m_links) {
		if (m >= 0) {
			if (reqURI(links[m].c_str()))
				body();
//			view.set(links[m].c_str());
			client.stop();
			pop();
		}
		if (m == APP_EXIT) {
			pop();
		}
		return 0;
	} else if (app() == &view && m == KEY_BS) {
		if (hist_size > 0) {
			hist_size --;
			if (reqURI(history[(--hist_pos)%hist_max].c_str(), false)) {
				body();
				view.show();
			}
		}
	}
	return m;
}

bool
Gemini::select()
{
	if (!server) {
		if (!reqURI("gemini://hugeping.ru"))
			return false;
		body();
		client.stop();
	}
	set(&view);
	return true;
}
