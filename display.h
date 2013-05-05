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
extern volatile uint64_t current_ticks;
extern volatile uint8_t current_ticks_lock;
inline void getCurrent_ticks_(uint64_t *ticks) {
	uint8_t l;
	do {
		l = current_ticks_lock;
		*ticks = current_ticks;
	} while (l!=current_ticks_lock);
}
inline uint64_t getCurrent_ticks(void) {
	uint64_t ticks;
	getCurrent_ticks_(&ticks);
	return ticks;
}

void setupDisplay(void);
void clearBuffer(volatile uint8_t *buffer);
signed char drawString(volatile uint8_t *buffer, signed char pos, const char *string);
void getCopperBars(uint8_t *color, uint16_t t);
void colorBar(volatile uint8_t *buffer, const uint8_t *color);

#endif // DISPLAY_H
