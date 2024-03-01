#include "external.h"
#include "internal.h"

static const char *wifi_cancel_menu[] = { "Ok", "Disconnect", NULL };
Wifilist::Wifilist(Screen &scr, Keyboard &kbd) : wifi_menu (scr, kbd, "WiFi", 64),
	wifi_cancel(scr, kbd, NULL, wifi_cancel_menu),
	wifi_pass(scr, kbd, "Password", 128),
	info(scr, kbd, "Info")
{
	wifi_pass.oneline = true;
	wifi_pass.set("");
	wifi_cancel.h = 2;
	wifi_cancel.y = ROWS-2;
}

int
Wifilist::process()
{
	int m = app->process();
	if (app == &wifi_menu && m >= 0) {
		app = &wifi_pass;
		app->select();
	} else if (app == &wifi_cancel) {
		if (m == APP_EXIT || m == 0)
			return APP_EXIT;
		if (m == 1) { /* cancel */
			WiFi.disconnect(true);
			return APP_EXIT;
		}
	} else if (app == &wifi_pass) {
		if (m == APP_EXIT)
			return APP_EXIT;
		if (m == KEY_ENTER) {
			WiFi.begin(wifi_menu.selected(), wifi_pass.text());
			return APP_EXIT;
		}
	}
	return 0;
}

bool
Wifilist::select()
{
	static const char *statuses[] = { "Idle",
		"No SSID", "SCAN", "CONNECTED", "FAILED",
		"LOST", "DISCONNECTED" };
	int status = WiFi.status();
	if (status != WL_NO_SHIELD) {
		char fmt[128];
		sprintf(fmt, "SSID: %s\nIP: %s\nStatus: %s",
			WiFi.SSID().c_str(),
			WiFi.localIP().toString().c_str(),
			statuses[status]);
		info.set(fmt);
		info.show();
		wifi_cancel.sel = 0;
		app = &wifi_cancel;
		app->select();
		return true;
	}
	wifi_menu.reset();
	wifi_menu.append("Подождите...");
	wifi_menu.show();
	app = &wifi_menu;
	int nr = WiFi.scanNetworks();
	wifi_menu.reset();
	for (int net = 0; net < nr; net ++)
		wifi_menu.append(WiFi.SSID(net).c_str());
	wifi_menu.show();
	return true;
}
