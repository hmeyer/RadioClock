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

extern volatile uint8_t *drawBuffer;
extern volatile uint8_t switchBuffersFlag;
inline void _getCurrent_ticks(uint32_t *ticks) {
	extern volatile uint32_t current_ticks;
	extern volatile uint8_t current_ticks_lock;
	uint8_t l;
	do {
		l = current_ticks_lock;
		*ticks = current_ticks;
	} while (l!=current_ticks_lock);
}
inline uint32_t getCurrent_ms(void) {
	uint32_t t; _getCurrent_ticks(&t);
	return t<<2;
}

void setupDisplay(void);
void clearBuffer(volatile uint8_t *buffer);
signed char drawString(volatile uint8_t *buffer, signed char pos, const char *string);
void getCopperBars(uint8_t *color, uint16_t t);
void colorBar(volatile uint8_t *buffer, const uint8_t *color);

#endif // DISPLAY_H
