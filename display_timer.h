#ifndef DISPLAY_TIMER_H
#define DISPLAY_TIMER_H

#include <stdint.h>

inline void _getCurrent_ticks(uint32_t *ticks) {
	extern volatile uint32_t current_ticks;
	extern volatile uint8_t current_ticks_lock;
	uint8_t l;
	do {
		l = current_ticks_lock;
		*ticks = current_ticks;
	} while (l!=current_ticks_lock);
}
inline uint32_t getMilliSeconds(void) {
	uint32_t t; _getCurrent_ticks(&t);
	return t;
}

#endif // DISPLAY_TIMER_H
