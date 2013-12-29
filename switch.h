#ifndef SWITCH_H
#define SWITCH_H

#include <avr/sfr_defs.h>
#include <avr/io.h>
#include "wiring.h"

#define SW_PIN 5

namespace sw {
enum Button {
	up = 1,
	down = 4,
	left = 0,
	first = 0,
	right = 2,
	center = 3,
	none = 6,
	last = none
};
}
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

inline bool buttonPressed(sw::Button b) {
	if (_buttonPressed & _BV(b)) {
		_buttonPressed &= ~_BV(b);
		return true;
	}
	return false;
}

inline sw::Button getButton() {
	sw::Button b = sw::first;
	while( (b != sw::last) 
		&& !buttonPressed(b)) b=static_cast<sw::Button>(1+(int)b);
	return b;
}

#endif
