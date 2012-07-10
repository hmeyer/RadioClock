#include <avr/interrupt.h>
#include <stdint.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include "display.h"
#include "wiring.h"
#include "character.h"
#include "myspi.h"

#define redMask 0b01010101
#define greenMask (redMask<<1)
#define orangeMask (redMask|greenMask)


#define COL_ST 3

#define ROW_ST 16
#define ROW_D 15
#define ROW_CP 14
#define ROW_OE 17

volatile uint8_t displayBuff[3 * XRES * 2];
volatile uint8_t drawBuff[3 * XRES * 2];
volatile uint8_t *displayBuffer = displayBuff;
volatile uint8_t *drawBuffer = drawBuff;
volatile uint8_t switchBuffersFlag=0;

volatile uint8_t line = 255;
volatile uint8_t frame = 0;
volatile uint32_t globalMS = 0;


#define usToCYCLES(microseconds) ((F_CPU / 2000000) * microseconds)
const uint16_t DisplayTimerCycles[] = {usToCYCLES(120), usToCYCLES(200), usToCYCLES(400)};

inline void LEDdisable() {
	digitalWrite(ROW_OE, false);
}
inline void LEDenable() {
	digitalWrite(ROW_OE, true);
}
#define storeLine(stat) digitalWrite(COL_ST, stat)

#define rowshift(v) {\
  if (v)\
    digitalWrite(ROW_D, 1);\
  else \
    digitalWrite(ROW_D, 0);\
  digitalWrite(ROW_CP, 1);\
  digitalWrite(ROW_CP, 0);\
}

void getCopperBars(uint8_t *color, uint16_t t) {
  for(int y=0;y<8;y++) {
    uint8_t mask = 0;
    uint16_t tg = (-t+y*3)*13;
    uint16_t tr = (t+y*5)*11;
    uint8_t vg = cosine(tg & 255)>>6;
    uint8_t vr = cosine(tr & 255)>>6;
    mask |= (vg<<2) | vr;
    color[y] = mask;
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
    if (*string >= '0' && *string <= '9')
      charIdx = *string - '0';
    drawChar( buffer, pos, charIdx);
    string++;
  }
  return pos;
}


inline void setDisplayTimer(uint8_t iteration) {
  uint16_t cycles = DisplayTimerCycles[iteration];
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    ICR1 = cycles;
  } 
  globalMS += cycles/8;
}

void initDisplayTimer(void) {
  TCCR1A = 0;                 // clear control register A 
  TCCR1B = _BV(WGM13);        // set mode 8: phase and frequency correct pwm, stop the timer
  
  setDisplayTimer(2);
  TIMSK1 = _BV(TOIE1);// sets the timer overflow interrupt enable bit

  // Set Clock Prescaler to NO Prescaler
  uint8_t clockSelectBits = _BV(CS10);
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= clockSelectBits;   
}


void setupDisplay(void) {
  pinMode(ROW_OE, OUTPUT);
  LEDdisable();
  pinMode(COL_ST, OUTPUT);
  digitalWrite(COL_ST, false);
  pinMode(ROW_D, OUTPUT);
  pinMode(ROW_CP, OUTPUT);
  pinMode(ROW_ST, OUTPUT);
  digitalWrite(ROW_ST, true);
  
  SPIsetup();
  for(int i = 0; i < 8; i++) {
    SPItransfer(0);
    rowshift(true);
  }
  initDisplayTimer();
}




inline void fillLineShift(void) {
  volatile uint8_t *linebuffer = &(displayBuffer[ frame * (XRES*2) + line * (2 * XRES / 8)]);\
  SPItransferBufferReverse(linebuffer, 2 * XRES / 8);
}





//void displayCallback() {
ISR(TIMER1_OVF_vect) {
  LEDdisable();
  storeLine(true);
  rowshift(line != 0);
  LEDenable();
  if (!line) setDisplayTimer(frame);

  line++;
  if (line == 8) {
    line = 0;
    frame++; 
    if ( frame == 3 ) {
      frame = 0;
      if (switchBuffersFlag) {
        volatile uint8_t *t = drawBuffer;
        drawBuffer = displayBuffer;
        displayBuffer = t;
        switchBuffersFlag = 0;
      }
    }
  }
  storeLine(false);
  fillLineShift();
}

