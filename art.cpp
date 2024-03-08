#include "external.h"
#include "internal.h"

Art::Art(Screen &screen, Keyboard &keys, WiFiClientSecure &c):
	scr(screen), kbd(keys), client(c)
{
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
Art::display(const char *title, uint8_t *buf)
{
	char fmt[256];
	sprintf(fmt, "%d/%d\n%s", start + 1, total, title);
	scr.clear();
	if (!buf)
		scr.text(0, 0, "Unsupported format");
	scr.text(0, ROWS-2, fmt);
	scr.update(true);
	if (!buf)
		return;
	scr.tft.setWindow(0, 0, 239, 191);
	for (int nr = 0; nr < 3; nr ++)
		display_block(nr, buf);
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

	StaticJsonDocument<512> json;
	DeserializationError error = deserializeJson(json, client);
	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		client.stop();
		return false;
	}
	total = json["totalAmount"];
	String url = json["responseData"]["zxPicture"][0]["originalUrl"];
	String title = json["responseData"]["zxPicture"][0]["title"];
	if (!url.startsWith("https://zxart.ee")) {
		client.stop();
		return false;
	}
	int i = 0;
	if (url.endsWith(".scr")) {
		sprintf(fmt, "GET %s HTTP/1.1", url.c_str()+16);
		Serial.println(fmt);
		http_request(fmt);
		while ((client.connected() || client.available()) && i < 6912) {
			screen[i++] = client.read();
		}
		Serial.println();
		Serial.println("Bytes readed: " + String(i));
	}
	client.stop();
	display(title.c_str(), (i == 6912)?screen:NULL);
	return true;
}
int
Art::process()
{
	uint8_t c;
	while ((c = kbd.input())) {
		if (c == KEY_ESC) {
			scr.tft.fillRect(0, 0, W, H, 0);
			scr.update(true);
			return APP_EXIT;
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
	request();
	return true;
}
