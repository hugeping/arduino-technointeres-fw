#include "external.h"
#include "internal.h"

WiFiClientSecure sslClient;
WiFiClient client;

Screen scr = Screen();
Keyboard kbd = Keyboard();

Menu main_menu(scr, kbd, "Main", (const char *[]){ "WiFi", "Edit", "Gemini", "IRC", "Sensor", NULL });
App app(&main_menu);

Edit edit_box(scr, kbd, "Edit", 4096);
Wifilist wifi(scr, kbd);
Gemini gemini(scr, kbd, sslClient);
Sensor sensor(scr, kbd);
Irc irc(scr, kbd, client, sslClient);

void
setup()
{
	Serial.begin(115200);
	sslClient.setInsecure();
	scr.setup();
	kbd.setup();
	sensor.setup();
	wifi.setup();
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
		switch (m) {
		case 0:
			app.push(&wifi);
			break;
		case 1:
			app.push(&edit_box);
			break;
		case 2:
			app.push(&gemini);
			break;
		case 3:
			app.push(&irc);
			break;
		case 4:
			app.push(&sensor);
			break;
		default:
			break;
		}
	} else if (m == APP_EXIT) {
		app.pop();
	}
	status();
}
