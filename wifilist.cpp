#include "external.h"
#include "internal.h"

static const char *wifi_cancel_menu[] = { "Ok", "Disconnect", NULL };
Wifilist::Wifilist(Screen &scr, Keyboard &kbd) : m_wifi(scr, kbd, "WiFi", 64),
	m_cancel(scr, kbd, NULL, wifi_cancel_menu),
	e_pass(scr, kbd, "Password", 128),
	v_info(scr, kbd, "Info")
{
	e_pass.oneline = true;
	e_pass.set("");
	m_cancel.h = 2;
	m_cancel.y = ROWS-2;
}

int
Wifilist::process()
{
	int m = app()->process();
	if (app() == &m_wifi && m >= 0) {
		push(&e_pass);
	} else if (app() == &m_cancel) {
		if (m == APP_EXIT || m == 0)
			return APP_EXIT;
		if (m == 1) { /* cancel */
			WiFi.disconnect(true);
			return APP_EXIT;
		}
	} else if (app() == &e_pass) {
		if (m == APP_EXIT) {
			pop();
			return 0;
		}
		if (m == KEY_ENTER) {
			WiFi.begin(m_wifi.selected(), e_pass.text());
			return APP_EXIT;
		}
	}
	return m;
}

bool
Wifilist::select()
{
	reset();
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
		v_info.set(fmt);
		v_info.show();
		m_cancel.sel = 0;
		set(&m_cancel);
		return true;
	}
	WiFi.disconnect(true);
	m_wifi.reset();
	m_wifi.append("Подождите...");
	m_wifi.show();

	int nr = WiFi.scanNetworks();
	m_wifi.reset();
	for (int net = 0; net < nr; net ++)
		m_wifi.append(WiFi.SSID(net).c_str());
	set(&m_wifi);
	return true;
}
