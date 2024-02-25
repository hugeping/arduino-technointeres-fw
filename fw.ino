#include <Arduino.h>
#include <WiFi.h>
#include "screen.h"
#include "keys.h"
#include "utf8.h"
#include "menu.h"

Screen scr = Screen();
Keyboard kbd = Keyboard();

Menu main_menu(scr, kbd, "Main", (const char *[]){ "WiFi", "Irc", "Gemini", "Exit", NULL });
Menu wifi_menu(scr, kbd, "WiFi", 32);
Menu *menus[] = { &main_menu, &wifi_menu };
int menu_nr = 0;

void
setup()
{
	scr.setup();
	kbd.setup();
	main_menu.show();
}

void loop()
{
	kbd.poll();
	int m = menus[menu_nr]->process();
	if (m == 0 && menu_nr == 0) {
		menu_nr = 1;
		int nr = WiFi.scanNetworks();
		wifi_menu.len = 0;
		for (int net = 0; net < nr; net ++) {
			wifi_menu.append(WiFi.SSID(net).c_str());
		}
		wifi_menu.show();
	} else if (m == -2 && menu_nr == 1) {
		menu_nr = 0;
		scr.clear();
		main_menu.show();
	}
	scr.update();
}
