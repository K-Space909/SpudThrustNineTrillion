// ROCSI_OLED.h

#ifndef _ROCSI_OLED_h
#define _ROCSI_OLED_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

enum BitmapState { Ready, Crosshairs, PGraph };

class TATER_OLEDClass {

public:

	void init();
	//void drawBitmap(const uint8_t bmp[]);

	void drawBitmap(BitmapState bmp);

	//void write(int x, int y, const char* txt, int size);
	void write(int x, int y, char txt[], int size);
	void writeBlk(int x, int y, char txt[], int size);
	void writeln(int x, int y, char txt[], int size);
	void DisplayGraph(int p[], float C);
	void setPixel(int16_t x, int16_t y, uint16_t color);
	void displayNow();
	void clear();
	void Splash(float v);
	void trybitmap();

private:

};

#endif

