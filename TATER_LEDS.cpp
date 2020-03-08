#include "TATER_LEDS.h"

//LED pins
#define greenled  3
#define yellowled  5
#define redled  6   

void TATER_LEDClass::init() {

	pinMode(greenled, OUTPUT);
	pinMode(yellowled, OUTPUT);
	pinMode(redled, OUTPUT);
}

void TATER_LEDClass::setSolid(LEDColor color) { // No PWM

	init(); //re-init pins before all digital writes in case an analog write has previously been written. This turns off the PWM timer.
	switch (color) {

	case LEDColor::green:
		digitalWrite(greenled, HIGH);
		digitalWrite(yellowled, LOW);
		digitalWrite(redled, LOW);
		break;
	case LEDColor::yellow:
		digitalWrite(greenled, LOW);
		digitalWrite(yellowled, HIGH);
		digitalWrite(redled, LOW);
		break;
	case LEDColor::red:
		digitalWrite(greenled, LOW);
		digitalWrite(yellowled, LOW);
		digitalWrite(redled, HIGH);
		break;
	default:
		digitalWrite(greenled, LOW);
		digitalWrite(yellowled, LOW);
		digitalWrite(redled, LOW);
		break;
	}
}

void TATER_LEDClass::Flash(LEDColor color, int num) { // Note* Uses delays. //  Flash color, number of times to flash  colors are1 = red, 2 = green, 3 = blue, 4 = cyan, 5 = lime

	for (int i = 0; i < num; i++) {
		setSolid(color);
		delay(50);
		ledsout();
		delay(50);
	}
}

void TATER_LEDClass::setPWM(int g, int y, int r) {

	//if (g == 255) { pinMode(greenled, OUTPUT); digitalWrite(greenled, HIGH); }
	//else if (g == 0) { pinMode(greenled, OUTPUT); digitalWrite(greenled, LOW); }
	//else { analogWrite(greenled,  g); }
	analogWrite(greenled, g);

	//if (y == 255) { pinMode(yellowled, OUTPUT); digitalWrite(yellowled, HIGH); }
	//else if (y == 0) { pinMode(yellowled, OUTPUT); digitalWrite(yellowled, LOW); }
	//else { analogWrite(yellowled,  y); }
	analogWrite(yellowled, y);

	//if (r == 255) { pinMode(redled, OUTPUT); digitalWrite(redled, HIGH); }
	//else if (r == 0) { pinMode(redled, OUTPUT); digitalWrite(redled, LOW); }
	//else { analogWrite(redled,  r); }
	analogWrite(redled, r);
}

void TATER_LEDClass::fadeDemo(int ms) {

	int rVal = 254;
	int gVal = 1;
	int bVal = 127;

	int rDir = -5;
	int gDir = 5;
	int bDir = -5;

	long p = millis();
	long q = millis();

	while (q - p < ms) {
		q = millis();

		// PWM the LED
		analogWrite(greenled, rVal);
		analogWrite(yellowled, gVal);
		analogWrite(redled, bVal);

		// change the values 
		rVal = rVal + rDir;
		gVal = gVal + gDir;
		bVal = bVal + bDir;

		// for each color, change direction if you reached 0 or 255
		if (rVal >= 255 || rVal <= 0) { rDir = rDir * -1; }

		if (gVal >= 255 || gVal <= 0) { gDir = gDir * -1; }

		if (bVal >= 255 || bVal <= 0) { bDir = bDir * -1; }

		delay(10);
	}

	ledsout();
}

void TATER_LEDClass::ledsout() {

	init(); //re-init pins before all digital writes in case an analog write has previousluy been written. This turns off the PWM timer.

	digitalWrite(greenled, LOW);
	digitalWrite(yellowled, LOW);
	digitalWrite(redled, LOW);
}