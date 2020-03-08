// ROCSI_RGB.h

#ifndef _ROCSI_RGB_h
#define _ROCSI_RGB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

enum LEDColor { green, yellow, red };

class TATER_LEDClass {

public:

	void init();
	void ledsout();
	void setSolid(LEDColor color);               // No PWM: solid bright colors
	void Flash(LEDColor color, int num);         // Note* Uses delays. //  Flash color, number of times to flash.  
	void setPWM(int g, int y, int r);      //ints 0-255
	void fadeDemo(int ms);
};

#endif