#ifndef SWITCH_H
#define SWITCH_H

#include "wiring.h"

#define SW_LEFT 5
#define SW_UP 6
#define SW_RIGHT 7
#define SW_DOWN 18
#define SW_PUSH 19

#define SW_F_UP (1<<0)
#define SW_F_DOWN (1<<1)
#define SW_F_LEFT (1<<2)
#define SW_F_RIGHT (1<<3)
#define SW_F_PUSH (1<<4)

extern uint8_t switchState;
extern uint8_t switchPressedState;

inline void setupSwitch() {
	pinMode(SW_UP, INPUT);
	pinMode(SW_DOWN, INPUT);
	pinMode(SW_LEFT, INPUT);
	pinMode(SW_RIGHT, INPUT);
	pinMode(SW_PUSH, INPUT);
	digitalWrite(SW_UP, true);
	digitalWrite(SW_DOWN, true);
	digitalWrite(SW_LEFT, true);
	digitalWrite(SW_RIGHT, true);
	digitalWrite(SW_PUSH, true);
	switchState = 0;
	switchPressedState = 0;
}

inline void updateButton(uint8_t b) {
	uint8_t bf = 0;
	switch(b) {
		case SW_UP: bf = SW_F_UP; break;
		case SW_DOWN: bf = SW_F_DOWN; break;
		case SW_LEFT: bf = SW_F_LEFT; break;
		case SW_RIGHT: bf = SW_F_RIGHT; break;
		case SW_PUSH: bf = SW_F_PUSH; break;
	}
	if (!digitalRead(b)) {
		switchState |= bf;
	} else if (switchState & bf) {
		switchPressedState |= bf;
		switchState ^= bf;
	}
}

inline void updateSwitch(void) {
	updateButton(SW_UP);
	updateButton(SW_DOWN);
	updateButton(SW_LEFT);
	updateButton(SW_RIGHT);
	updateButton(SW_PUSH);
}

inline bool switchPressed(uint8_t flag) {
	if (switchPressedState & flag) {
		switchPressedState ^= flag;
		return true;
	}
	return false;
}

#endif
