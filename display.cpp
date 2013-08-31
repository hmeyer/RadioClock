#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "display.h"
#include "wiring.h"
#include "character.h"
#include "cosine.h"
#include "myspi.h"
#include "bits.h"
#include "switch.h"

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
volatile bool switchBuffersFlag=false;

volatile uint8_t line = 255;
volatile uint8_t lineBit = 0;
volatile uint8_t frame = 0;
volatile uint16_t current_us_err = 0;
volatile uint32_t current_ticks = 0;
volatile uint8_t current_ticks_lock;

volatile bool waitClear=true;
extern volatile bool DEBUG;


const uint16_t DisplayTimerCycles[] = { 800, 2000, 8000};

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

void LEDDisplay::getCopperBars(uint8_t *color, uint16_t t) {
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

void LEDDisplay::colorBar(const uint8_t *color) { // higher bits = background, lower bits = foreground
  for(int8_t i=3; i>=1; i--) {
    volatile uint8_t *lookupbuffer = drawBuffer;
    volatile uint8_t *fillbuffer = &(drawBuffer[XRES*2*(i-1)]);
    for(uint8_t y=0; y < 8; y++) {
      uint8_t col = color[y];
      uint8_t fgred = 0, fggreen =  0, bgred = 0, bggreen = 0;
      if (( col       & 3)      == i) fgred = 0xff;
      if (((col >> 2) & 3) == i) fggreen = 0xff;
      if (((col >> 4) & 3) == i) bgred = 0xff;
      if ( (col >> 6)      == i) bggreen = 0xff;
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


void LEDDisplay::clearBuffer() {
  for(uint8_t i=0;i < XRES * 2;i++)
    drawBuffer[i]=0;
}

void LEDDisplay::drawChar(signed char &pos, uint8_t charIdx) {
  const signed char width = 5;
  if ((pos > -width) && (pos < XRES)) {
    signed char bufferIndex = pos >> 3;
    uint8_t bitOffset = pos & 7;
    bool upperLimit = !((bufferIndex+1) >= (XRES/8));
    bool lowerLimit = (bufferIndex >= 0);
    for(uint8_t y=0;y<8;y++) {
      uint8_t charByte = charset(charIdx, y);
      if (lowerLimit) drawBuffer[ bufferIndex ] |= charByte >> bitOffset;
      if (bitOffset && upperLimit) {
        drawBuffer[ bufferIndex + 1 ] |= charByte << (8-bitOffset);
      }
      bufferIndex += 2 * (XRES/8);
    }
  }
  pos += width+1;
}

signed char LEDDisplay::drawString(signed char pos, const char *string) {
  while(*string!=0 && pos < XRES) {
    uint8_t charIdx = 10;
    char c = *string;
    if (c >= 32 && c <= 127)
      charIdx = c - 32;
    else charIdx = 127 - 32;
      
    drawChar( pos, charIdx);
    string++;
  }
  return pos;
}

void LEDDisplay::scrollString(const char *string, int16_t counter) {
	size_t l = strlen(string)*6; 
	int16_t scrollrange = l + XRES;
	if (scrollrange<1) {
		drawString(0, string);
		return;
	}
	int16_t scrollpos = counter % scrollrange;
	scrollpos = XRES - scrollpos;
	drawString(scrollpos, string);
}


void LEDDisplay::plot(uint32_t x, uint8_t y, uint8_t red, uint8_t green){
	volatile uint8_t *drawBuffer = DISP.getDrawBuffer();
	if(x<0) return;
	if(y<0) return;
	if(x>63) return;
	if(y>7) return;
	drawBuffer[       (y<<4) +      (x>>3) ] |= red   << ((63-x) & 7);
	//drawBuffer[4*XRES+(y<<4) +      (x>>3) ] |= red   << ((63-x) & 7);
	drawBuffer[       (y<<4) + 8 +  (x>>3) ] |= green << ((63-x) & 7);
	//drawBuffer[4*XRES+(y<<4) + 8 +  (x>>3) ] |= green << ((63-x) & 7);

}


inline void setDisplayTimer(uint16_t cycles) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    OCR1A = cycles;
  }
  if (!frame) ATOMIC_BLOCK(ATOMIC_FORCEON) {
    current_ticks_lock++;
    current_ticks+=11;
  }
}

void initDisplayTimer(void) {
  TCCR1A = 0;
  TCCR1B = _BV(WGM12);        // set CTC
  
  setDisplayTimer( DisplayTimerCycles[2] );
  TIMSK1 = _BV(OCIE1A);// sets the timer 1 output compare interrupt enable bit

  // Set Clock Prescaler to NO Prescaler
  uint8_t clockSelectBits = _BV(CS10);
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= clockSelectBits;   
}


void LEDDisplay::begin(void) {
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
  wdt_enable( WDTO_15MS );
}






inline void fillLineShift(void) {
  volatile uint8_t *linebuffer = &(displayBuffer[ frame * (XRES*2) + line * (2 * XRES / 8)]);\
  SPItransferBufferReverse(linebuffer, 2 * XRES / 8);
}





//void displayCallback() {
ISR(TIMER1_COMPA_vect) {
  if (!waitClear) {
  	LEDdisable_Clock();
  	waitClear = true;
  } else if (SPIfinished()) {
	  storeLine();
	  LEDenableShift_Clock(line);
	  updateButton(lineBit);
	  if (!line) setDisplayTimer( DisplayTimerCycles[frame] );
	  line++;
	  lineBit<<=1;
	  if (line == 8) {
	    line = 0;
	    lineBit = 1;
	    frame++; 
	    if ( frame == MAX_FRAMES ) {
	      frame = 0;
	      if (switchBuffersFlag) {
		volatile uint8_t *t = drawBuffer;
		drawBuffer = displayBuffer;
		displayBuffer = t;
		switchBuffersFlag = false;
	      }
	    }
	  }
  	  waitClear = false;
	  fillLineShift();
  }
}

LEDDisplay DISP;

