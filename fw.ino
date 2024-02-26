#include <Arduino.h>
#include <WiFi.h>
#include "screen.h"
#include "keys.h"
#include "utf8.h"
#include "menu.h"
#include "edit.h"

Screen scr = Screen();
Keyboard kbd = Keyboard();

Menu main_menu(scr, kbd, "Main", (const char *[]){ "WiFi list", "Text Playground", NULL });
Menu wifi_menu(scr, kbd, "WiFi", 32);
Edit edit_box(scr, kbd, "Edit", 4096);
App *apps[] = { &main_menu, &wifi_menu, &edit_box };
int app_nr = 0;

void
setup()
{
	scr.setup();
	kbd.setup();
	main_menu.show();
	edit_box.set("Привет, мир!");
}

void loop()
{
	kbd.poll();
	int m = apps[app_nr]->process();
	if (m == 0 && app_nr == 0) {
		app_nr = 1;
		wifi_menu.reset();
		wifi_menu.append("Подождите...");
		wifi_menu.show();
		scr.update();
		int nr = WiFi.scanNetworks();
		wifi_menu.reset();
		for (int net = 0; net < nr; net ++) {
			wifi_menu.append(WiFi.SSID(net).c_str());
		}
		wifi_menu.show();
	} else if(m == 1 && app_nr == 0) {
		app_nr = 2;
		scr.clear();
		edit_box.show();
	} else if (m == -2 && app_nr != 0) {
		app_nr = 0;
		scr.clear();
		main_menu.show();
	}
	scr.update();
}
