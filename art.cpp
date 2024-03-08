#include "external.h"
#include "internal.h"

Art::Art(Screen &screen, Keyboard &keys, WiFiClientSecure &c):
	scr(screen), kbd(keys), client(c), m_page(screen, keys, "Page", 16)
{
	m_page.oneline = true;
	m_page.y = ROWS - 3;
}

void
Art::setup()
{
	prefs.begin("art", true);
	start = prefs.getInt("page", 0);
	Serial.println("Get start:" + String(start));
	prefs.end();
}

void
Art::http_request(const char *req)
{
	client.println(req);
	client.println("Host: zxart.ee");
//	client.println("Connection: close");
	client.println();
	while (client.connected() || client.available()) { /* skip header */
		String l = client.readStringUntil('\n');
		if (l.isEmpty() || l == "\r")
			break;
	}
}

static uint8_t pal[16][3] = {
	{ 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0xd8 },
	{ 0xd8, 0x00, 0x00 },
	{ 0xd8, 0x00, 0xd8 },
	{ 0x00, 0xd8, 0x00 },
	{ 0x00, 0xd8, 0xd8 },
	{ 0xd8, 0xd8, 0x00 },
	{ 0xd8, 0xd8, 0xd8 },

	{ 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0xff },
	{ 0xff, 0x00, 0x00 },
	{ 0xff, 0x00, 0xff },
	{ 0x00, 0xff, 0x00 },
	{ 0x00, 0xff, 0xff },
	{ 0xff, 0xff, 0x00 },
	{ 0xff, 0xff, 0xff },
};

void
Art::display_block(int nr, uint8_t *buf)
{
	uint8_t *attr = buf + 0x1800;
	for (int yy = 0; yy < 8; yy ++) {
		uint8_t *pa = attr + (nr*8 + yy)*32;
		uint8_t *p = buf + nr*2048 + 32*yy;
		for (int i = 0; i < 8; i ++) {
			for (int xx = 0; xx < 32; xx ++) {
				uint8_t b = *p++;
				if (xx == 0 || xx == 31)
					continue;
				uint8_t a = pa[xx];
				int br = (a&0x40)?8:0;
				uint8_t *fc = pal[(a & 0x7) + br];
				uint8_t *bc = pal[((a>>3) & 0x7) + br];
				color_t fg = scr.color(fc[0], fc[1], fc[2]);
				color_t bg = scr.color(bc[0], bc[1], bc[2]);
				for (int s = 0; s < 8; s++) {
					if (b & (0x80 >> s))
						scr.tft.pushBlock(fg, 1);
					else
						scr.tft.pushBlock(bg, 1);
				}
			}
			p += 256-32;
		}
	}

}

void
Art::message(const char *msg)
{
	scr.clear();
	scr.text(0, 0, msg, FG, true);
	scr.update(true);
}

void
Art::display(const char *title, uint8_t *buf)
{
	char fmt[256];
	sprintf(fmt, "%d/%d\n%s", start + 1, total, title);
	scr.clear();
	if (!buf)
		scr.text(0, 0, "Wrong data", FG, true);
	scr.text(0, ROWS-3, fmt, 0x0000ff, true);
	scr.update(true);
	if (!buf)
		return;
	scr.tft.setWindow(0, 0, 239, 191);
	for (int nr = 0; nr < 3; nr ++)
		display_block(nr, buf);
	prefs.begin("art", false);
	prefs.putInt("page", start);
	prefs.end();
}
static struct {
	const char *pfx;
	const char s;
} amp_sub[] = {
	{ "amp;", '&' },
	{ "quot;", '"' },
	{ "lt;", '<' },
	{ "gt;", '>' },
	{ NULL, 0 },
};

static void
decode(char *str)
{
	char c;
	char *dst = str;
	while ((c = *str++)) {
		if (c == '&') {
			if (str[0] == '#') {
				int pos = strcspn(str, ";");
				if (str[pos]) {
					str[pos] = 0;
					*dst ++ = atoi(str + 1);
					str += pos;
					continue;
				}
			}
			bool hit = false;
			for (int i = 0; amp_sub[i].pfx; i++) {
				int len = strlen(amp_sub[i].pfx);
				if (!strncmp(str, amp_sub[i].pfx, len)) {
					*dst ++ = amp_sub[i].s;
					str += len;
					hit = true;
					break;
				}
			}
			if (hit)
				continue;
		}
		*dst ++ = c;
		continue;
	}
	*dst = 0;
}

static void
url_encode(char *dst, const char *src)
{
	while (*src) {
		if (*src == '#') {
			sprintf(dst, "%%%02x", *src);
			dst += 3;
			src ++;
		} else
			*dst ++ = *src ++;
	}
	*dst = *src;
}
bool
Art::request()
{
	char fmt[256];
	static uint8_t screen[6912];
	Serial.println("Connect...");
	if (!client.connect("zxart.ee", 443)) {
		Serial.println("Connection error");
		return false;
	}
	// ,desc
	sprintf(fmt, "GET /api/types:zxPicture/export:zxPicture/start:%d/limit:1/order:date,asc/filter:zxPictureType=standard; HTTP/1.1",
		start);
	http_request(fmt);
	Serial.println("Req done");
	StaticJsonDocument<512> json;
	DeserializationError error = deserializeJson(json, client);
	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		client.stop();
		return false;
	}
	total = json["totalAmount"];
	Serial.println("Total:"+String(total));
	char url[256];
	const char *v = json["responseData"]["zxPicture"][0]["originalUrl"];
	strcpy(url, v?v:"");
	decode(url);
//	Serial.println(url);
	if (strncmp(url, "https://zxart.ee", 16)) {
		client.stop();
		display("", NULL);
		return true;
	}
	url_encode(fmt, url);
	strcpy(url, fmt);

	char title[256];
	v = json["responseData"]["zxPicture"][0]["title"];
	strcpy(title, v?v:"");
	decode(title);
	int i = 0;

	sprintf(fmt, "GET %s HTTP/1.1", url+16);
	Serial.println(fmt);
	http_request(fmt);
	while ((client.connected() || client.available()) && i < 6912)
		screen[i++] = client.read();
//	Serial.println("Bytes readed: " + String(i));

	client.stop();
	display(title, (i == 6912)?screen:NULL);
	return true;
}
int
Art::process()
{
	uint8_t c;
	if (app() == &m_page) {
		int m = m_page.process();
		if (m == KEY_ENTER) {
			start = atoi(m_page.text())-1;
			start = min(total - 1, start);
			start = max(0, start);
			reset();
			request();
		} else if (m == APP_EXIT) {
			reset();
			scr.clear(0, ROWS-3, COLS, 3);
			scr.update();
		}
		return 0;
	}
	while ((c = kbd.input())) {
		if (c == KEY_ESC) {
			scr.tft.fillRect(0, 0, W, H, 0);
			scr.update(true);
			return APP_EXIT;
		} else if (c == KEY_ENTER || c == KEY_MENU) {
			scr.clear(0, ROWS-3, COLS, 3);
			m_page.set(String(start+1).c_str());
			set(&m_page);
			return 0;
		}
		if (c == KEY_RIGHT || c == KEY_DOWN) {
			start += (c == KEY_DOWN)?10:1;
		} else if (c == KEY_LEFT || c == KEY_UP) {
			start -= (c == KEY_UP)?10:1;
		}
		start = min(start, total-1);
		start = max(0, start);
		request();
	}
	return 0;
}

bool
Art::select()
{
	if (!request()) {
		reset();
		message("Connection error");
	}
	return true;
}
