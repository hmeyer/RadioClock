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



volatile long ledcntr = 0;
volatile uint8_t bits = 0;

#define COL_D 11
#define LINE_CLK 13
#define COL_OE 2
#define COL_ST 3

#define ROW_OE 8
#define ROW_ST 7
#define ROW_D 6
#define ROW_CP 5

volatile uint8_t displayBuff[3 * XRES * 2];
volatile uint8_t drawBuff[3 * XRES * 2];
volatile uint8_t *displayBuffer = displayBuff;
volatile uint8_t *drawBuffer = drawBuff;
volatile uint8_t switchBuffersFlag=0;

volatile uint8_t line = 255;
volatile uint8_t lineiters = 0;


#define usToCYCLES(microseconds) ((uint16_t)(F_CPU / 2000000) * microseconds)
#define cyclesTOus(cycles) ((uint32_t)(cycles * 2000000) / F_CPU)
const uint16_t DisplayTimerCycles[] = {usToCYCLES(85), usToCYCLES(170), usToCYCLES(340)};

#define linedisable() digitalWrite(COL_OE, false)
#define lineenable() digitalWrite(COL_OE, true)
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
      uint8_t bgmask = 0, fgmask = 0;
      if ((col >> 6) >  i) bgmask |= greenMask;
      if (((col >> 4)&3) >  i) bgmask |= redMask;
      if (((col >> 2)&3) >  i) fgmask |= greenMask;
      if (((col)&3) >  i) fgmask |= redMask;
      for(uint8_t x=0; x < XRES/4; x++) {
        uint8_t val = lookupbuffer[x];
        fillbuffer[x] = (val & fgmask) | ((~val)&bgmask);
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
#ifdef DEBUG
  Serial.print("drawChar:"); Serial.println(pos);
#endif
  const signed char width = 5;
  if ((pos > -width) && (pos < XRES)) {
    signed char bufferIndex = ((pos+4) / 4) - 1;
    uint8_t bitOffset = (pos*2+8) & 7;
    bool upperLimit = !((bufferIndex+1) >= (XRES/4));
    bool lowerLimit = (bufferIndex >= 0);
#ifdef DEBUG
  Serial.print("bufferIndex:"); Serial.println(bufferIndex);
#endif
    for(uint8_t y=0;y<8;y++) {
      uint8_t charByte = charset(charIdx, y);
      uint8_t highB = mono2HighColorByte(charByte);
      uint8_t lowB = mono2LowColorByte(charByte);
      if (lowerLimit) buffer[ bufferIndex ] |= highB >> bitOffset;
      if (upperLimit) {
        if (bitOffset) buffer[ bufferIndex + 1 ] |= highB << (8-bitOffset);
        buffer[ bufferIndex+1 ] |= lowB >> bitOffset;
      }
      bufferIndex += XRES/4;
    }
  }
  pos += width+1;
}

signed char drawString(volatile uint8_t *buffer, signed char pos, const char *string) {
#ifdef DEBUG
    Serial.println("drawString:");
#endif
  while(*string!=0 && pos < XRES) {
    uint8_t charIdx = 10;
#ifdef DEBUG
    Serial.print(pos);
    Serial.print(":");
    Serial.print(*string);
    Serial.print(" ");
#endif
    if (*string >= '0' && *string <= '9')
      charIdx = *string - '0';
    drawChar( buffer, pos, charIdx);
    string++;
  }
#ifdef DEBUG
    Serial.println();
#endif
  return pos;
}


#define setDisplayTimer(iteration) \
  uint16_t cycles = DisplayTimerCycles[iteration];\
  ATOMIC_BLOCK(ATOMIC_FORCEON) {\
    ICR1 = cycles;\
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
  pinMode(COL_D, OUTPUT);
  pinMode(LINE_CLK, OUTPUT);
  pinMode(COL_OE, OUTPUT);
  pinMode(COL_ST, OUTPUT);
  digitalWrite(COL_OE, false);
  digitalWrite(COL_ST, false);
  pinMode(ROW_D, OUTPUT);
  pinMode(ROW_CP, OUTPUT);
  pinMode(ROW_OE, OUTPUT);
  pinMode(ROW_ST, OUTPUT);
  digitalWrite(ROW_OE, false);
  digitalWrite(ROW_ST, true);
  
  SPIsetup();
  for(int i = 0; i < 8; i++) {
    SPItransfer(0);
    rowshift(true);
  }
  digitalWrite(ROW_OE, true);
  initDisplayTimer();
}




#define fillLineShift() \
  volatile uint8_t *linebuffer = &(displayBuffer[ lineiters * XRES*2 + line*XRES/4]);\
  SPItransferBufferReverse(linebuffer, XRES/4);





//void displayCallback() {
ISR(TIMER1_OVF_vect) {
#ifdef DEBUG
    Serial.println("displayCallback");
#endif
  linedisable();
  storeLine(true);
  if (!lineiters) rowshift(line != 0);
  lineenable();
  setDisplayTimer(lineiters);

  lineiters++;
  if (lineiters == 3) {
    lineiters = 0;
    line++; 
    if ( line == 8 ) {
      line = 0;
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

