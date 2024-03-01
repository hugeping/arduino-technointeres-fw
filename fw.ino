#include "external.h"
#include "internal.h"

WiFiClientSecure sslClient;

Screen scr = Screen();
Keyboard kbd = Keyboard();

Menu main_menu(scr, kbd, "Main", (const char *[]){ "WiFi", "Edit", "Gemini", "Sensor", NULL });
App app(&main_menu);

Edit edit_box(scr, kbd, "Edit", 4096);
Wifilist wifi(scr, kbd);
Gemini gemini(scr, kbd, sslClient);
Sensor sensor(scr, kbd);

void
setup()
{
	sslClient.setInsecure();
	scr.setup();
	kbd.setup();
	sensor.setup();
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
