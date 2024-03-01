#include "external.h"
#include "internal.h"

WiFiClientSecure sslClient;

Screen scr = Screen();
Keyboard kbd = Keyboard();

Menu main_menu(scr, kbd, "Main", (const char *[]){ "WiFi", "Edit", "Gemini", "Sensor", NULL });
Edit edit_box(scr, kbd, "Edit", 4096);
Wifilist wifi(scr, kbd);
Gemini gemini(scr, kbd, sslClient);

Sensor sensor_app(scr, kbd);

App *apps[16];
int app_nr = 0;

void
push_app(App *a)
{
	if (!a->select())
		return;
	apps[app_nr++] = a;
}

void
set_app(App *a)
{
	if (app_nr == 0)
		app_nr ++;
	apps[app_nr-1] = a;
	a->select();
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
	sslClient.setInsecure();
	scr.setup();
	kbd.setup();
	sensor_app.setup();
	main_menu.show();
	//edit_box.set("Привет, мир!");
	push_app(&main_menu);
}
void
status()
{
}

void loop()
{
	kbd.poll();
	int m = app()->process();
	if (app() == &main_menu && m >= 0) {
		switch (m) {
		case 0:
			push_app(&wifi);
			break;
		case 1:
			push_app(&edit_box);
			break;
		case 2:
			push_app(&gemini);
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
