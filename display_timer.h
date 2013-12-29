#ifndef DISPLAY_TIMER_H
#define DISPLAY_TIMER_H

//#include <stdint.h>
#include <util/atomic.h>

/*
inline void _getCurrent_ticks(uint32_t *ticks) {
	extern volatile uint32_t current_ticks;
	extern volatile uint8_t current_ticks_lock;
	uint8_t l;
	uint8_t c=0;
	do {
		l = current_ticks_lock;
		*ticks = current_ticks;
		c++;
	} while ((l!=current_ticks_lock)&&(c<10));
}
*/
inline static uint32_t getMilliSeconds(void) {
	uint32_t ticks;
	//_getCurrent_ticks(&ticks);
	
	extern volatile uint32_t current_ticks;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ticks = current_ticks;
	}
	
	return ticks;
}

#endif // DISPLAY_TIMER_H
