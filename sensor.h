#ifndef __SENSOR_H_INCLUDED
#define __SENSOR_H_INCLUDED
#include "app.h"
#include "keys.h"
#include "screen.h"
#include <SHT3x.h>

class Sensor : public App {
	SHT3x sensor;
	Screen &scr;
	Keyboard &kbd;
	int t;
	int h;
public:
	Sensor(Screen &scr, Keyboard &kbd);
	void setup();
	int process();
	void show();
};
#endif
