// Visual Micro is in vMicro>General>Tutorial Mode
/*
    Name:       TATERS_GON_TATE.ino
    Created:	1/2/2020 9:51:48 PM
    Author:     D.M.KONYNDYK
*/

#include <ESC.h>
#include <movingAvg.h>
#include "TATER_LEDS.h"
#include "TATER_OLED.h"
#include <EEPROM.h>

/*############################CONTROLS########################################################*/
const float version = 0.0;
int nFuel = 20;                      // n for rolling averages on fuel pressure transducer
int TargetPressure;          // Tuneable target PSI for fuel rail
int ChamberThreshold = 20;   //chamber pressure PSI that indicates a shot is being fired
int dumptime = 5000;         //time to keep fuel dump solenoid open
int exhaustTime = 1000;     //time to blow exhaust
/*############################CONTROLS########################################################*/

const int ESCpin = 2;
const int fuelPin_rail = 4;
const int fuelPin_dump = A2;
const int ingitionPin = A3;
const int purgePin = A4;
const int buttonPin_1 = 7;         //active low
const int buttonPin_2 = 8;         //active high
const int pressurePin_Fuel = A1;
const int pressurePin_Chamber = A0;
const int safetyPin = A5;          //active high when safety off  (use pulldown)
const int triggerPin = A6;         //active high when trigger pulled (use pulldown)
const int joystickPin = A7; 
float FuelPressure;          // Fuel ral pressure in PSI
float ChamberPressure;       // Chamber pressure in PSI
unsigned long A;
unsigned long B;
bool safeState = false;   // true when safety on. initialize to true
bool armstate = true;     // true when fully ready to fire. Initialize to true
bool fuelState;           // true indicates fuel rail reached target pressure
bool fuelTimeout = false;    // true indicates fuel proccess did not reach target pressure
bool switcht = true;         //true when just switched into a new mode
int Button;
int laststickval;
int MODE = 0;          // main operating mode defaults to manual at boot
enum buttons {none, one, two, onetwo, up, down, left, right, in, TRIG};
enum modes {manual, automatic, menu};
#define DEBUG true
#define fueladdress 30700                          //address to store fuel target PSI
#define SPEED_MIN (1000)                            // Set the Minimum Speed in microseconds
#define SPEED_MAX (2000)                            // Set the Minimum Speed in microseconds
ESC mixESC(ESCpin, SPEED_MIN, SPEED_MAX, 500);      // ESC_Name (ESC PIN, Minimum Value, Maximum Value, Default Speed, Arm Value)
TATER_OLEDClass OLED;
TATER_LEDClass LED;
movingAvg fuelPressure(nFuel);
//movingAvg chamberPressure(nFuel);

void setup()
{
	if (DEBUG) { Serial.begin(9600); }

	pinMode(ESCpin, OUTPUT);
	pinMode(fuelPin_rail, OUTPUT);
	digitalWrite(fuelPin_rail, HIGH);
	pinMode(fuelPin_dump, OUTPUT);
	digitalWrite(fuelPin_dump, HIGH);
	pinMode(ingitionPin, OUTPUT);
	digitalWrite(ingitionPin, HIGH);
	pinMode(purgePin, OUTPUT);
	digitalWrite(purgePin, HIGH);
	pinMode(buttonPin_1, INPUT_PULLUP);
	pinMode(buttonPin_2, INPUT_PULLUP);
	pinMode(pressurePin_Fuel, INPUT);
	pinMode(pressurePin_Chamber, INPUT);
	pinMode(safetyPin, INPUT);
	pinMode(triggerPin, INPUT);
	pinMode(joystickPin, INPUT);
	LED.init();
	OLED.init();
	mixESC.arm();   //arm ESC
	mixESC.speed(0);
	//mixESC.stop();  

	LED.fadeDemo(2000);
	LED.setPWM(10, 10, 10);
	OLED.Splash(version);

	fuelPressure.begin();
	//chamberPressure.begin();
	//EEPROM.write(fueladdress, 18);
	TargetPressure = EEPROM.read(fueladdress);

	if(DEBUG) Serial.println(F("setup done"));

	
}

/*################################################################################################################*/

void loop()
{
	B = millis();

	GetInputs();

	switch (MODE) {
	case modes::manual:{ //////////////////////////////////////MANUAL///////////////MANUAL/////////////MANUAL/////////////MANUAL/////////
		
		if (switcht) {
			LED.setPWM(100,0,0);
			OLED.clear();
			OLED.write(10, 30, "MANUAL", 3);
			delay(1000);
			OLED.clear();
			if (fuelState == false) { OLED.write(2, 10, "<  FUEL", 1); }
			else{ OLED.writeBlk(2, 10, "<  FUEL", 1); }
			OLED.write(2, 55, "<  PURGE/EXHAUST", 1);
			switcht = false;
		}

		if (Button == one && !fuelState) {
			OLED.writeBlk(2, 10, "<  FUEL", 1);
			OLED.write(2, 10, "CHARGING FUEL", 1);
			chargeFuel();
			if (fuelState) {
				LED.setPWM(0, 100, 0); 
				OLED.writeBlk(2, 10, "CHARGING FUEL", 1);
				dumpFuel();
				LED.setSolid(red);
			}
		}

		if (Button == two) {
			OLED.writeBlk(2, 55, "<  PURGE/EXHAUST", 1);
			Serial.println(fuelState);
			if (fuelState) { clearRail(); fuelState = false;}
			Purge(exhaustTime);
			OLED.write(2, 10, "<  FUEL", 1);
			LED.setPWM(100, 0, 0);
			OLED.write(2, 55, "<  PURGE/EXHAUST", 1);

		}



	}
	break;

	case modes::automatic: {

	}
	break;

	case modes::menu: {

	}
	break;

	}


}

void RunSHOT(){}

void chargeFuel() {

	if (DEBUG) Serial.println(F("PRESSURIZING FUEL"));

	digitalWrite(fuelPin_rail, LOW);

	int iter = 0;
	int A = millis();
	int B = millis();

	while (FuelPressure < TargetPressure  && (B - A) < 15000) {

		int fuelDivs = fuelPressure.reading(analogRead(pressurePin_Fuel));   // calculate the fuel rail pressure moving average 
		FuelPressure = ((fuelDivs * 0.0049) - 0.5) / 0.01333;

		B = millis();
		iter++;
		if (DEBUG && iter % 20 == 0 ) {
			Serial.print(F("FUEL RAIL PSI: "));
			Serial.println(FuelPressure);
		}
	}
	digitalWrite(fuelPin_rail, HIGH);
	if (B - A >= 12000) { fuelState = false; fuelTimeout = true; }  //fueling timeout- pressure not reached
	else { fuelState = true; LED.setPWM(0, 100, 0);}

}

void dumpFuel() {

	if (DEBUG) Serial.println("DUMPING FUEL");
	digitalWrite(fuelPin_dump, LOW);
	delay(dumptime);
	digitalWrite(fuelPin_dump, HIGH);

	if (DEBUG) Serial.println("CHAMBER FULLY ARMED ");
	LED.setSolid(red);
	armstate = true;
	fuelState = false;
	OLED.write(60, 30, "ARMED", 2);

}

void clearRail() {

	if (DEBUG) Serial.println("DUMPING FUEL");
	if (fuelState) {
		digitalWrite(fuelPin_dump, LOW);
		delay(dumptime);
		digitalWrite(fuelPin_dump, HIGH);
	}

	if (DEBUG) Serial.println("CHAMBER CLEARED");
	LED.setSolid(green);
	armstate = false;
	fuelState = false;
	OLED.writeBlk(60, 30, "ARMED", 2);

}

void Purge(int exhaustTime) {
	digitalWrite(purgePin, HIGH);
	delay(exhaustTime);
	digitalWrite(purgePin, LOW);

}

void GetInputs() {

	delay(25);

	if (analogRead(safetyPin) > 800) { safeState = false; }
	else { safeState = true; }

	if (analogRead(triggerPin) > 800 && safeState == false) {
		Button = TRIG;
	}

	else { Button = none; }

	int fuelDivs = fuelPressure.reading(analogRead(pressurePin_Fuel));   // calculate the fuel rail pressure moving average 
	int chamberDivs = analogRead(pressurePin_Chamber);   // Take the instantaneous chanber pressure
	FuelPressure = ((fuelDivs * 0.0049) - 0.5) / 0.01333; 
	ChamberPressure = ((chamberDivs * 0.0049) - 0.5) / 0.01333;

	if (FuelPressure >= TargetPressure) { fuelState = true; Serial.println(FuelPressure); }
	if (ChamberPressure >= ChamberThreshold) { RunSHOT(); }

	check2Buttons();
	checkJoystick();
   
	if (Button && DEBUG) {
		//Serial.print(analogRead(joystickPin));
		//Serial.print(F(",  "));
		Serial.print(F("Button: "));
		Serial.println(Button);
	}
}

void check2Buttons() {
	if (!digitalRead(buttonPin_1)) {  //buttons 1 and 2 hold the thread to catch dual button press
		Button = one;
		while (!digitalRead(buttonPin_1)) {
			while (digitalRead(buttonPin_2) && !digitalRead(buttonPin_1)) {
				Button = onetwo;
			}
		}
		delay(20);//debounce
	}
	if (digitalRead(buttonPin_2)) {  //buttons 1 and 2 hold the thread to catch dual button press
		Button = two;
		while (digitalRead(buttonPin_2)) {
			while (digitalRead(buttonPin_2) && !digitalRead(buttonPin_1)) {
				Button = onetwo;
			}
		}
		delay(20); //debounce
	}
}

void checkJoystick() {

	int stickval = analogRead(joystickPin);

	if (stickval > 20 && abs(stickval - laststickval) > 20) {
		if (300 < stickval && stickval < 420) { Button = up;  } //4
		if (600 < stickval && stickval < 999) { Button = down;  } //5
		if (180 < stickval && stickval < 300) { Button = left;  } //6
		if (420 < stickval && stickval < 600) { Button = right;  } //7
		if (100 < stickval && stickval < 180) { Button = in;  } //8
	}
	laststickval = stickval;
	
}



