#include "external.h"
#include "internal.h"

Gemini::Gemini(Screen &scr, Keyboard &kbd, WiFiClientSecure &c) : view(scr, kbd, "Gemini"), client(c)
{
}

int
Gemini::process()
{
	return app->process();
}

bool
Gemini::select()
{
	if (!client.connect("hugeping.ru", 1965))
		return false;
	client.println("gemini://hugeping.ru");
	String out;
	while (client.connected() || client.available()) {
		String line = client.readStringUntil('\n');
		out += line + "\n";
	}
	client.stop();
	view.set(out.c_str());
	app = &view;
	app->select();
	return true;
}
