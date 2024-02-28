#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "screen.h"
#include "keys.h"
#include "utf8.h"
#include "menu.h"
#include "edit.h"
#include "view.h"
#include "sensor.h"

Screen scr = Screen();
Keyboard kbd = Keyboard();
WiFiClientSecure client;

Menu main_menu(scr, kbd, "Main", (const char *[]){ "WiFi", "Edit", "Gemini", "Sensor", NULL });
Menu wifi_menu(scr, kbd, "WiFi", 32);
Edit edit_box(scr, kbd, "Edit", 4096);
Edit wifi_pass(scr, kbd, "Password", 128);
View info(scr, kbd, "Info");
Menu wifi_cancel(scr, kbd, NULL, (const char *[]) { "Ok", "Disconnect", NULL });

static char fmt[256];

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
	if (app_nr<2)
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
	wifi_pass.oneline = true;
	wifi_pass.set("");
	wifi_cancel.h = 2;
	wifi_cancel.y = ROWS-2;
	client.setInsecure();
}
void
status()
{
}
void
after_wifi_passwd(int m)
{
	if (m == APP_EXIT) {
		pop_app();
		return;
	}
	if (m == KEY_ENTER) {
		WiFi.begin(wifi_menu.selected(), wifi_pass.text());
		pop_app();
	}
}

void
after_wifi_cancel(int m)
{
	if (m == APP_EXIT || m == 0) {
		pop_app();
		return;
	}
	if (m == 1) { /* cancel */
		wifi_cancel.sel = 0;
		WiFi.disconnect(true);
		pop_app();
		return;
	}
}

void
select_wifi(int m)
{
	static const char *statuses[] = { "Idle", "No SSID", "SCAN", "CONNECTED", "FAILED", "LOST", "DISCONNECTED" };
	int status = WiFi.status();
	if (status != WL_NO_SHIELD) {
		sprintf(fmt, "SSID: %s\nIP: %s\nStatus: %s",
			WiFi.SSID().c_str(),
			WiFi.localIP().toString().c_str(),
			statuses[status]);
		info.set(fmt);
		info.show();
		push_app(&wifi_cancel);
		return;
	}
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

void loop()
{
	kbd.poll();
	int m = app()->process();
	if (app() == &wifi_menu && m >= 0) {
		set_app(&wifi_pass);
	} else if (app() == &wifi_cancel) {
		after_wifi_cancel(m);
	} else if (app() == &wifi_pass) {
		after_wifi_passwd(m);
	} else if (app() == &main_menu && m >= 0) {
		switch (m) {
		case 0:
			select_wifi(m);
			break;
		case 1:
			scr.clear();
			push_app(&edit_box);
			break;
		case 2:
			if (client.connect("hugeping.ru", 1965)) {
				client.println("gemini://hugeping.ru");
				String out;
				while (client.connected() || client.available()) {
					String line = client.readStringUntil('\n');
					out += line + "\n";
				}
				client.stop();
				info.set(out.c_str());
				push_app(&info);
			}
			break;
		case 3:
			push_app(&sensor_app);
			break;
		default:
			break;
		}
	} else if (m == APP_EXIT) {
		pop_app();
	}
	status();
}
