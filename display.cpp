#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include "display.h"
#include "wiring.h"
#include "character.h"
#include "cosine.h"
#include "myspi.h"
#include "bits.h"

#define redMask 0b01010101
#define greenMask (redMask<<1)
#define orangeMask (redMask|greenMask)


#define COL_ST 3
#define COL_CLK 4


#define MAX_FRAMES 3

volatile uint8_t displayBuff[MAX_FRAMES * XRES * 2];
volatile uint8_t drawBuff[MAX_FRAMES * XRES * 2];
volatile uint8_t *displayBuffer = displayBuff;
volatile uint8_t *drawBuffer = drawBuff;
volatile uint8_t switchBuffersFlag=0;

volatile uint8_t line = 255;
volatile uint8_t frame = 0;
volatile uint16_t current_us_err = 0;
volatile uint64_t current_ticks = 0;
volatile uint8_t current_ticks_lock;

volatile uint8_t waitClear=0;
extern volatile bool DEBUG;


#define usToCYCLES(microseconds) ((F_CPU / 2000000) * microseconds)
const uint16_t DisplayTimerUs[] = {120, 200, 400, 60};
const uint16_t DisplayTimerCycles[] = {usToCYCLES(DisplayTimerUs[0]), 
				usToCYCLES(DisplayTimerUs[1]),
				usToCYCLES(DisplayTimerUs[2]),
				usToCYCLES(DisplayTimerUs[3])};

inline void LEDdisable_Clock() {
	wdt_reset();
	digitalWrite(ROW_OE, false);
}
inline void LEDenableShift_Clock(uint8_t v) {
	digitalWrite(ROW_CP, 0);
	digitalWrite(ROW_D, v);
	digitalWrite(ROW_CP, 1);
	digitalWrite(ROW_OE, true);
}

inline void storeLine(void) {
	digitalWrite(COL_ST, true);
	digitalWrite(COL_ST, false);
}

void getCopperBars(uint8_t *color, uint16_t t) {
  for(int y=0;y<8;y++) {
    uint8_t mask = 0;
    uint16_t tg = (-t+y*3)*13;
    uint16_t tr = (t+y*5)*11;
    uint8_t vg = cosine(tg & 255)>>6;
    uint8_t vr = cosine(tr & 255)>>6;
    mask |= (vg<<2) | vr;
    color[y] = 15;
//    color[y] = mask;
  }
}

void colorBar(volatile uint8_t *buffer, const uint8_t *color) { // higher bits = background, lower bits = foreground
  for(int8_t i=2; i>=0; i--) {
    volatile uint8_t *lookupbuffer = buffer;
    volatile uint8_t *fillbuffer = &(buffer[XRES*2*i]);
    for(uint8_t y=0; y < 8; y++) {
      uint8_t col = color[y];
      uint8_t fgred = 0, fggreen =  0, bgred = 0, bggreen = 0;
      if (( col       & 3)       > i) fgred = 0xff;
      if (((col >> 2) & 3) > i) fggreen = 0xff;
      if (((col >> 4) & 3) > i) bgred = 0xff;
      if ( (col >> 6)      > i) bggreen = 0xff;
      uint8_t x2 = XRES/8;
      for(uint8_t x=0; x < XRES/8; x++) {
        uint8_t val = lookupbuffer[x];
	fillbuffer[x] = (val & fgred) | ((~val) & bgred);
	fillbuffer[x2] = (val & fggreen) | ((~val) & bggreen);
	x2++;
      }
      lookupbuffer+=XRES/4;
      fillbuffer+=XRES/4;
    }
  }
}


void clearBuffer(volatile uint8_t *buffer) {
  for(uint8_t i=0;i < XRES * 2;i++)
    buffer[i]=0;
}

void drawChar(volatile uint8_t *buffer, signed char &pos, uint8_t charIdx) {
  const signed char width = 5;
  if ((pos > -width) && (pos < XRES)) {
    signed char bufferIndex = pos >> 3;
    uint8_t bitOffset = pos & 7;
    bool upperLimit = !((bufferIndex+1) >= (XRES/8));
    bool lowerLimit = (bufferIndex >= 0);
    for(uint8_t y=0;y<8;y++) {
      uint8_t charByte = charset(charIdx, y);
      if (lowerLimit) buffer[ bufferIndex ] |= charByte >> bitOffset;
      if (bitOffset && upperLimit) {
        buffer[ bufferIndex + 1 ] |= charByte << (8-bitOffset);
      }
      bufferIndex += 2 * (XRES/8);
    }
  }
  pos += width+1;
}

signed char drawString(volatile uint8_t *buffer, signed char pos, const char *string) {
  while(*string!=0 && pos < XRES) {
    uint8_t charIdx = 10;
    char c = *string;
    if (c >= 32 && c <= 127)
      charIdx = c - 32;
    else charIdx = 127 - 32;
      
    drawChar( buffer, pos, charIdx);
    string++;
  }
  return pos;
}


inline void setDisplayTimer(uint8_t frame) {
  uint16_t cycles =  DisplayTimerCycles[frame];
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    ICR1 = cycles;
  }
  uint16_t us = DisplayTimerUs[frame];
  current_us_err += us;
  if (current_us_err >= 1000) {
    current_us_err -= 1000;
    current_ticks_lock++;
    current_ticks++;
  }
}

void initDisplayTimer(void) {
  TCCR1A = 0;                 // clear control register A 
  TCCR1B = _BV(WGM13);        // set mode 8: phase and frequency correct pwm, stop the timer
  
  setDisplayTimer( 2 );
  TIMSK1 = _BV(TOIE1);// sets the timer overflow interrupt enable bit

  // Set Clock Prescaler to NO Prescaler
  uint8_t clockSelectBits = _BV(CS10);
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= clockSelectBits;   
}


void setupDisplay(void) {
  pinMode(ROW_OE, OUTPUT);
  LEDenableShift_Clock(true);
  LEDdisable_Clock();
  pinMode(COL_ST, OUTPUT);
  digitalWrite(COL_ST, false);
  pinMode(ROW_D, OUTPUT);
  pinMode(ROW_CP, OUTPUT);
  pinMode(ROW_ST, OUTPUT);
  digitalWrite(ROW_ST, true);
  
  SPIsetup();
  for(int i = 0; i < 8; i++) {
    SPItransfer(0);
  }
  initDisplayTimer();
  wdt_enable( WDTO_500MS );
}




inline void fillLineShift(void) {
  volatile uint8_t *linebuffer = &(displayBuffer[ frame * (XRES*2) + line * (2 * XRES / 8)]);\
  SPItransferBufferReverse(linebuffer, 2 * XRES / 8);
}





//void displayCallback() {
ISR(TIMER1_OVF_vect) {
  waitClear ^= 1;
  if (waitClear) {
  	LEDdisable_Clock();
  	setDisplayTimer(3);
  } else {
	  storeLine();
	  LEDenableShift_Clock(line);
	  if (!line) setDisplayTimer( frame );

	  line++;
	  if (line == 8) {
	    line = 0;
	    frame++; 
	    if ( frame == MAX_FRAMES ) {
	      frame = 0;
	      if (switchBuffersFlag) {
		volatile uint8_t *t = drawBuffer;
		drawBuffer = displayBuffer;
		displayBuffer = t;
		switchBuffersFlag = 0;
	      }
	    }
	  }
	  fillLineShift();
  }
}

