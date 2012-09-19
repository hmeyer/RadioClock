#ifndef BITS_H
#define BITS_H
#include <stdint.h>
#include <avr/interrupt.h>
#include "wiring.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#endif
