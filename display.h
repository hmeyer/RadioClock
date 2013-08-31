#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#define XRES 64
#define ROW_ST 16
#define ROW_D 15
#define ROW_CP 14
#define ROW_OE 17

#define COL_ST 3
#define COL_CLK 4
#define COL_D 1


class LEDDisplay {
	private:
	void drawChar(signed char &pos, uint8_t charIdx);
	public:
	void begin(void);
	void clearBuffer();
	signed char drawString(signed char pos, const char *string);
	void scrollString(const char *string, int16_t counter);
	void getCopperBars(uint8_t *color, uint16_t t);
	void colorBar(const uint8_t *color);
	void plot(uint32_t x, uint8_t y, uint8_t red, uint8_t green);
	inline volatile uint8_t *getDrawBuffer() {
		extern volatile uint8_t *drawBuffer;
		return drawBuffer;
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
};

extern LEDDisplay DISP;

#endif // DISPLAY_H
