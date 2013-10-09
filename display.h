#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "rtc/RTClib.h"

#define XRES 64
#define ROW_ST 16
#define ROW_D 15
#define ROW_CP 14
#define ROW_OE 17

#define COL_ST 3
#define COL_CLK 4
#define COL_D 1

#define MAX_FRAMES 3

#define RED     3
#define GREEN  12
#define YELLOW 15

class LEDDisplay {
	private:
	uint8_t textColor;
	int8_t  scrollSpeed;
	uint8_t leftBorder;
	uint8_t rightBorder;
	int16_t brightness;

	public:


	void begin(void);

	void clearBuffer();

	int16_t drawChar(char character, int16_t pos);
	int8_t drawNumber(int8_t pos, uint8_t number, bool leadingZero, bool align);

	inline void setTextColor(uint8_t color){
		textColor=color;
	}

	inline void setScrollSpeed(uint8_t speed){
		scrollSpeed=speed;
	}
	inline uint8_t getScrollSpeed(){
		return scrollSpeed;
	}

	inline void resetBorder(){
		leftBorder=0;
		rightBorder=XRES;
	}

	inline void setBorder(uint8_t left, uint8_t right){
		leftBorder=left;
		rightBorder=right;
	}

	int16_t drawString(int16_t pos, const char *string);

	int8_t printDate(int8_t pos, uint8_t hour, uint8_t minute, uint8_t second);
	int8_t printDuration(int8_t pos, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

	void scrollString(const char *string);

	//void getCopperBars(uint8_t *color, uint16_t t);
	//void colorBar(const uint8_t *color);

	void rotateStar();
	//void softScreen();

	inline void plot(int8_t x, int8_t y);
	inline void plot(int8_t x, int8_t y, int8_t red, int8_t green);

	inline uint8_t getColor(int8_t x, int8_t y, int8_t colorChannel);
	inline uint8_t getRed(int8_t x, int8_t y);
	inline uint8_t getGreen(int8_t x, int8_t y);

	void copyBuffer(uint8_t *line);

	inline volatile uint8_t *getDrawBuffer() {
		extern volatile uint8_t *drawBuffer;
		return drawBuffer;
	}

	inline volatile uint8_t *getDisplayBuffer() {
		extern volatile uint8_t *displayBuffer;
		return displayBuffer;
	}

	inline void flush(void) {
		extern volatile bool switchBuffersFlag;
		switchBuffersFlag = true;
	}
	inline bool flushing(void) {
		extern volatile bool switchBuffersFlag;
		return switchBuffersFlag;
	}
	inline void waitUntilFlushed(void) {
		extern volatile bool switchBuffersFlag;
		while(switchBuffersFlag==true);
	}

	inline void setBrightness(uint8_t i){
		if((i>=0)&&(i<MAX_FRAMES)) brightness=i*2*XRES;
	}

	inline uint8_t getBrightness(){
		return brightness;
	}

	inline void increaseBrightness(){
		clearBuffer();
		if(brightness<MAX_FRAMES*2*XRES-1) brightness+=2*XRES;
	}

	inline void decreaseBrightness(){
		clearBuffer();
		if(brightness>0) brightness-=2*XRES;
	}


};

extern LEDDisplay display;

#endif // DISPLAY_H
