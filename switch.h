#ifndef SWITCH_H
#define SWITCH_H

#include <avr/sfr_defs.h>
#include <avr/io.h>
#include "wiring.h"

#define SW_PIN 5

#define SW_UP 1
#define SW_DOWN 4
#define SW_LEFT 0
#define SW_RIGHT 2
#define SW_PUSH 3

extern volatile uint8_t buttonBuffer;
extern volatile uint8_t _buttonPressed;

inline void setupSwitch() {
	pinMode(SW_PIN, INPUT);
	digitalWrite(SW_PIN, true);
	buttonBuffer = 0;
	_buttonPressed = 0;
}

inline uint8_t swPin(void) {
	return PIND & _BV(5);
}

inline void updateButton(uint8_t flag) {
	if (!swPin()) {
		buttonBuffer |= flag;
	} else {
		if (buttonBuffer & flag) {
			_buttonPressed |= flag;
			buttonBuffer ^= flag;
		}
	}
}

inline bool buttonPressed(uint8_t b) {
	if (_buttonPressed & _BV(b)) {
		_buttonPressed &= ~_BV(b);
		return true;
	}
	return false;
}

#endif
