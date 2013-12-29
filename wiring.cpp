#include "wiring.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>


#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define digitalPinToPort(P) ( pgm_read_byte( digital_pin_to_port_PGM + (P) ) )
#define digitalPinToBitMask(P) ( pgm_read_byte( digital_pin_to_bit_mask_PGM + (P) ) )
#define digitalPinToTimer(P) ( pgm_read_byte( digital_pin_to_timer_PGM + (P) ) )
#define analogInPinToBit(P) (P)
#define portOutputRegister(P) ( (volatile uint8_t *)( pgm_read_word( port_to_output_PGM + (P))) )
#define portInputRegister(P) ( (volatile uint8_t *)( pgm_read_word( port_to_input_PGM + (P))) )
#define portModeRegister(P) ( (volatile uint8_t *)( pgm_read_word( port_to_mode_PGM + (P))) )
#define NOT_A_PIN 0
#define NOT_A_PORT 0

#ifdef ARDUINO_MAIN
#define PA 1
#define PB 2
#define PC 3
#define PD 4
#define PE 5
#define PF 6
#define PG 7
#define PH 8
#define PJ 10
#define PK 11
#define PL 12
#endif

#define NOT_ON_TIMER 0
#define TIMER0A 1
#define TIMER0B 2
#define TIMER1A 3
#define TIMER1B 4
#define TIMER2  5
#define TIMER2A 6
#define TIMER2B 7

#define TIMER3A 8
#define TIMER3B 9
#define TIMER3C 10
#define TIMER4A 11
#define TIMER4B 12
#define TIMER4C 13
#define TIMER4D 14      
#define TIMER5A 15
#define TIMER5B 16
#define TIMER5C 17

#define PA 1
#define PB 2
#define PC 3
#define PD 4
#define PE 5
#define PF 6
#define PG 7
#define PH 8
#define PJ 10
#define PK 11
#define PL 12


extern const uint8_t digital_pin_to_bit_mask_PGM[] PROGMEM;
const uint8_t digital_pin_to_bit_mask_PGM[] = {
        _BV(0), /* 0, port D */
        _BV(1),
        _BV(2),
        _BV(3),
        _BV(4),
        _BV(5),
        _BV(6),
        _BV(7),
        _BV(0), /* 8, port B */
        _BV(1),
        _BV(2),
        _BV(3),
        _BV(4),
        _BV(5),
        _BV(0), /* 14, port C */
        _BV(1),
        _BV(2),
        _BV(3),
        _BV(4),
        _BV(5),
};

extern const uint8_t digital_pin_to_port_PGM[] PROGMEM;
const uint8_t digital_pin_to_port_PGM[] = {
        PD, /* 0 */
        PD,
        PD,
        PD,
        PD,
        PD,
        PD,
        PD,
        PB, /* 8 */
        PB,
        PB,
        PB,
        PB,
        PB,
        PC, /* 14 */
        PC,
        PC,
        PC,
        PC,
        PC,
};

extern const uint16_t port_to_mode_PGM[] PROGMEM;
const uint16_t port_to_mode_PGM[] = {
        NOT_A_PORT,
        NOT_A_PORT,
        (uint16_t) &DDRB,
        (uint16_t) &DDRC,
        (uint16_t) &DDRD,
};

extern const uint8_t digital_pin_to_timer_PGM[] PROGMEM;
const uint8_t digital_pin_to_timer_PGM[] = {
        NOT_ON_TIMER, /* 0 - port D */
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        // on the ATmega168, digital pin 3 has hardware pwm
#if defined(__AVR_ATmega8__)
        NOT_ON_TIMER,
#else
        TIMER2B,
#endif
        NOT_ON_TIMER,
        // on the ATmega168, digital pins 5 and 6 have hardware pwm
#if defined(__AVR_ATmega8__)
        NOT_ON_TIMER,
        NOT_ON_TIMER,
#else
        TIMER0B,
        TIMER0A,
#endif
        NOT_ON_TIMER,
        NOT_ON_TIMER, /* 8 - port B */
        TIMER1A,
        TIMER1B,
#if defined(__AVR_ATmega8__)
        TIMER2,
#else
        TIMER2A,
#endif
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        NOT_ON_TIMER, /* 14 - port C */
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        NOT_ON_TIMER,
};

extern const uint16_t port_to_output_PGM[] PROGMEM;
const uint16_t port_to_output_PGM[] = {
        NOT_A_PORT,
        NOT_A_PORT,
        (uint16_t) &PORTB,
        (uint16_t) &PORTC,
        (uint16_t) &PORTD,
};

extern const uint16_t port_to_input_PGM[] PROGMEM;
const uint16_t port_to_input_PGM[] = {
        NOT_A_PORT,
        NOT_A_PORT,
        (uint16_t) &PINB,
        (uint16_t) &PINC,
        (uint16_t) &PIND,
};





void pinMode(uint8_t pin, uint8_t mode) {
        uint8_t bit = digitalPinToBitMask(pin);
        uint8_t port = digitalPinToPort(pin);
        volatile uint8_t *reg;

        if (port == NOT_A_PIN) return;

        // JWS: can I let the optimizer do this?
        reg = portModeRegister(port);

        if (mode == INPUT) {
  		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                	*reg &= ~bit;
		}
        } else {
  		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                	*reg |= bit;
		}
        }
}

static void turnOffPWM(uint8_t timer)
{
        switch (timer)
        {
                #if defined(TCCR1A) && defined(COM1A1)
                case TIMER1A:   cbi(TCCR1A, COM1A1);    break;
                #endif
                #if defined(TCCR1A) && defined(COM1B1)
                case TIMER1B:   cbi(TCCR1A, COM1B1);    break;
                #endif

                #if defined(TCCR2) && defined(COM21)
                case  TIMER2:   cbi(TCCR2, COM21);      break;
                #endif

                #if defined(TCCR0A) && defined(COM0A1)
                case  TIMER0A:  cbi(TCCR0A, COM0A1);    break;
                #endif

                #if defined(TIMER0B) && defined(COM0B1)
                case  TIMER0B:  cbi(TCCR0A, COM0B1);    break;
                #endif
                #if defined(TCCR2A) && defined(COM2A1)
                case  TIMER2A:  cbi(TCCR2A, COM2A1);    break;
                #endif
                #if defined(TCCR2A) && defined(COM2B1)
                case  TIMER2B:  cbi(TCCR2A, COM2B1);    break;
                #endif

                #if defined(TCCR3A) && defined(COM3A1)
                case  TIMER3A:  cbi(TCCR3A, COM3A1);    break;
                #endif
                #if defined(TCCR3A) && defined(COM3B1)
                case  TIMER3B:  cbi(TCCR3A, COM3B1);    break;
                #endif
                #if defined(TCCR3A) && defined(COM3C1)
                case  TIMER3C:  cbi(TCCR3A, COM3C1);    break;
                #endif

                #if defined(TCCR4A) && defined(COM4A1)
                case  TIMER4A:  cbi(TCCR4A, COM4A1);    break;
                #endif
                #if defined(TCCR4A) && defined(COM4B1)
                case  TIMER4B:  cbi(TCCR4A, COM4B1);    break;
                #endif
                #if defined(TCCR4A) && defined(COM4C1)
                case  TIMER4C:  cbi(TCCR4A, COM4C1);    break;
                #endif
                #if defined(TCCR5A)
                case  TIMER5A:  cbi(TCCR5A, COM5A1);    break;
                case  TIMER5B:  cbi(TCCR5A, COM5B1);    break;
                case  TIMER5C:  cbi(TCCR5A, COM5C1);    break;
                #endif
        }
}


void digitalWrite(uint8_t pin, bool val)
{
        uint8_t timer = digitalPinToTimer(pin);
        uint8_t bit = digitalPinToBitMask(pin);
        uint8_t port = digitalPinToPort(pin);
        volatile uint8_t *out;

        if (port == NOT_A_PIN) return;

        // If the pin that support PWM output, we need to turn it off
        // before doing a digital write.
        if (timer != NOT_ON_TIMER) turnOffPWM(timer);

        out = portOutputRegister(port);

  	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (val == false) {
			*out &= ~bit;
		} else {
			*out |= bit;
		}
	}
}

bool digitalRead(uint8_t pin)
{
        uint8_t timer = digitalPinToTimer(pin);
        uint8_t bit = digitalPinToBitMask(pin);
        uint8_t port = digitalPinToPort(pin);

        if (port == NOT_A_PIN) return false;

        // If the pin that support PWM output, we need to turn it off
        // before getting a digital reading.
        if (timer != NOT_ON_TIMER) turnOffPWM(timer);

        if (*portInputRegister(port) & bit) return true;
        return false;
}

