#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "rtc/RTClib.h"
#include <util/atomic.h>
//#include "debug.h"

#define XRES 64
#define YRES 8
#define MAX_FRAMES 2
#define FRAME_SIZE 2*XRES
#define LINE_SIZE FRAME_SIZE/8

#define ROW_ST 16
#define ROW_D 15
#define ROW_CP 14
#define ROW_OE 17

#define COL_ST 3
#define COL_CLK 4
#define COL_D 1

#define BLACK   0
#define RED     1
#define GREEN   2
#define YELLOW  3

#define round(x) int(x+0.5)


class LedDisplay {
	private:
	volatile uint8_t displayBuff[MAX_FRAMES * FRAME_SIZE];
	volatile uint8_t drawBuff   [MAX_FRAMES * FRAME_SIZE];
	volatile uint8_t red;
	volatile uint8_t green;
	volatile uint8_t leftBorder;
	volatile uint8_t rightBorder;

	public:
	volatile uint8_t  frame;
	volatile uint8_t  scrollSpeed;
	volatile uint16_t brightness;
	volatile uint8_t  lock;
	volatile bool     waitClear;
	volatile bool     switchBuffer;
	volatile uint8_t  *displayBuffer;
	volatile uint8_t  *drawBuffer;

	void init(void);

	inline void setColor(uint8_t color){
		if (color==RED){
			red=1;
			green=0;
		}else if (color==GREEN){
			red=0;
			green=1;
		}else if (color==YELLOW){
			red=1;
			green=1;
		}else {
			red=0;
			green=0;
		}
	}


	void    clearBuffer();
	uint8_t drawChar(const char character, const int16_t pos);
	uint8_t drawNumber(const int16_t pos, const uint8_t number, const bool leadingZero, const bool align);
	uint8_t getCharWidth(const char c);

	uint16_t getStringSize(const char *string);
	int16_t drawString(volatile int16_t pos, const char *string);
	void    drawStringCenter(const char *string);

	int16_t printDate(int16_t pos);
	int16_t printDuration(int16_t pos, const uint8_t day, const uint8_t hour, const uint8_t minute, const uint8_t second);

	void scrollString(const char *string);

	inline void setScrollSpeed(const uint8_t speed){
		scrollSpeed=speed;
	}

	void blink();

	void showProgressBar(uint8_t progress);

	inline void resetBorder(){
		leftBorder=0;
		rightBorder=XRES-1;
	}

	inline void setBorder(const uint8_t left, const uint8_t right){
		if(left>right)return;
		if((left>=0) and (left<XRES))  leftBorder=left;
		if((right>=0) and (right<XRES)) rightBorder=right;
	}

	void rotateStar();

	void plot(const int16_t x, const int16_t y);

	void copyBuffer(const char *line);

	inline void flush(void) {
		switchBuffer = true;
	}
	inline bool flushing(void) {
		return switchBuffer;
	}
	inline void waitUntilFlushed(void) {
		while(switchBuffer==true);
	}

	inline void switchBuffers(void){
		//ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			volatile uint8_t  *temp;
			temp              = drawBuffer;
			drawBuffer        = displayBuffer;
			displayBuffer     = temp;
		//}
	}

	inline void setBrightness(const uint8_t i){
		if((i>=0) and (i<MAX_FRAMES-1)){
			clearBuffer();
			brightness=i*FRAME_SIZE;
		}
	}

	inline void increaseBrightness(){
		if(brightness<((MAX_FRAMES-1)*FRAME_SIZE)){
			clearBuffer();
			brightness+=FRAME_SIZE;
		}
	}

	inline void decreaseBrightness(){
		if(brightness>0){
			clearBuffer();
			brightness-=FRAME_SIZE;
		}
	}
};

extern LedDisplay display;

#endif // DISPLAY_H
