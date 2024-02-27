#include <Arduino.h>
#include <WiFi.h>
#include "screen.h"
#include "keys.h"
#include "utf8.h"
#include "menu.h"
#include "edit.h"
#include "sensor.h"

Screen scr = Screen();
Keyboard kbd = Keyboard();

Menu main_menu(scr, kbd, "Main", (const char *[]){ "WiFi list", "Text Playground", "Sensor", NULL });
Menu wifi_menu(scr, kbd, "WiFi", 32);
Edit edit_box(scr, kbd, "Edit", 4096);
Sensor sensor_app(scr, kbd);

App *apps[16];
int app_nr = 0;

void
push_app(App *a)
{
	apps[app_nr++] = a;
	a->show();
}

App *
pop_app()
{
	if (app_nr<=1)
		return NULL;
	scr.clear();
	App *a = apps[--app_nr];
	apps[app_nr-1]->show();
	return a;
}

App *
app()
{
	return apps[app_nr -1];
}

void
setup()
{
	scr.setup();
	kbd.setup();
	sensor_app.setup();
	main_menu.show();
	edit_box.set("Привет, мир!");
	push_app(&main_menu);
}

void loop()
{
	kbd.poll();
	int m = app()->process();
	if (app() == &main_menu && m >= 0) {
		switch (m) {
		case 0:
		{
			wifi_menu.reset();
			wifi_menu.append("Подождите...");
			push_app(&wifi_menu);
			int nr = WiFi.scanNetworks();
			wifi_menu.reset();
			for (int net = 0; net < nr; net ++) {
				wifi_menu.append(WiFi.SSID(net).c_str());
			}
			wifi_menu.show();
		}
			break;
		case 1:
			scr.clear();
			push_app(&edit_box);
			break;
		case 2:
			push_app(&sensor_app);
			break;
		default:
      break;
		}
	} else if (m == -2) {
		pop_app();
	}
}
