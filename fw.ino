#include <Arduino.h>
#include <WiFi.h>
#include "screen.h"
#include "keys.h"
#include "utf8.h"
#include "menu.h"
#include "edit.h"
#include "view.h"
#include "sensor.h"

Screen scr = Screen();
Keyboard kbd = Keyboard();

Menu main_menu(scr, kbd, "Main", (const char *[]){ "WiFi list", "Text Playground", "Sensor", NULL });
Menu wifi_menu(scr, kbd, "WiFi", 32);
Edit edit_box(scr, kbd, "Edit", 4096);
Edit wifi_pass(scr, kbd, "Password", 128);
View info(scr, kbd, "Info");

Sensor sensor_app(scr, kbd);

App *apps[16];
int app_nr = 0;

void
push_app(App *a)
{
	apps[app_nr++] = a;
	a->show();
}

void
set_app(App *a)
{
	if (app_nr == 0)
		app_nr ++;
	apps[app_nr-1] = a;
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
	//edit_box.set("Привет, мир!");
	push_app(&main_menu);
}

void loop()
{
	kbd.poll();
	int m = app()->process();
	if (app() == &wifi_menu && m >= 0) {
		set_app(&wifi_pass);
	} else if (app() == &wifi_pass && m == APP_EXIT) {
		WiFi.begin(wifi_menu.selected(), wifi_pass.text());
		pop_app();
	} else if (app() == &main_menu && m >= 0) {
		switch (m) {
		case 0:
		{
			if (WiFi.status() == WL_CONNECTED) {
				info.set(WiFi.localIP().toString().c_str());
				push_app(&info);
				break;
			} else {
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
	} else if (m == APP_EXIT) {
		pop_app();
	}
}
