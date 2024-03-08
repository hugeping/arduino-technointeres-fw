#include "external.h"
#include "internal.h"

WiFiClientSecure sslClient;
WiFiClient client;

Screen scr = Screen();
Keyboard kbd = Keyboard();

Menu main_menu(scr, kbd, "Main", (const char *[]){ "WiFi", "Edit", "Gemini", "IRC",
	"Sensor", "Art", "Settings", NULL });
App app(&main_menu);

Edit edit_box(scr, kbd, "Edit", 4096);
Wifilist wifi(scr, kbd);
Gemini gemini(scr, kbd, sslClient);
Art art(scr, kbd, sslClient);
Sensor sensor(scr, kbd);
Irc irc(scr, kbd, client, sslClient);
Settings settings(scr, kbd);

static App *menu_apps[] = { &wifi, &edit_box, &gemini, &irc, &sensor, &art, &settings };

void
setup()
{
	Serial.begin(115200);
	sslClient.setInsecure();
	scr.setup();
	kbd.setup();
	sensor.setup();
	wifi.setup();
	art.setup();
	settings.setup();
	app.show();
}

void
status()
{
}

void loop()
{
	kbd.poll();
	int m = app.process();
	if (app.app() == &main_menu && m >= 0) {
		Serial.println(String(m));
		app.push(menu_apps[m]);
	} else if (m == APP_EXIT) {
		app.pop();
	}
	status();
}
