#include "TimerOne.h"
#include "character.h"
#include "SPI.h"


volatile long ledcntr = 0;
volatile byte bits = 0;

#define COL_D 11
#define LINE_CLK 13
#define COL_OE 9
#define COL_ST 8

#define ROW_OE 4
#define ROW_ST 5
#define ROW_D 6
#define ROW_CP 7

#define XRES 40

//#define DEBUG 

volatile byte linebuffer[ XRES * 2];
volatile byte displaybufferG[ XRES * 2 ];
volatile byte displaybufferR[ XRES * 2 ];

volatile byte line = 255;
volatile byte lineiters = 0;

int t = 0;

void displayTestImage(void) {
  for(int y = 0; y < 8; y++) {
    for(int x = 0; x < XRES/4; x++) {
      displaybufferR[y*XRES/4 + x] = testChar1[y*2 + (x&1) ];
      displaybufferG[y*XRES/4 + x] = testChar2[y*2 + (x&1) ];
    }
  }
#ifdef DEBUG
    Serial.println("displayTestImage:");
    for(int y = 0; y < 8; y++) {
      for(int x = 0; x < XRES/4; x++) {
        Serial.print(displaybufferG[y*XRES/4 + x]);
        Serial.print(" ");
      }
      Serial.println();
    }
#endif  
}

void calculateBuffer(byte line) { 
#ifdef DEBUG
    Serial.print("calculateBuffer[");Serial.print(line); Serial.print("]:");
#endif
  byte didx = line * XRES / 4;
  byte lidx = 0;
  while( lidx < XRES *2 ) {
    byte r = displaybufferR[ didx ];
    byte g = displaybufferG[ didx++ ];
    for(byte i = 0; i < 4; i++) {
      linebuffer[ lidx++ ] = (r & 3);
      linebuffer[ lidx++ ] = (g & 3);
      r >>= 2;
      g >>= 2;
    }
  }
#ifdef DEBUG
    for(byte i=0; i < 16; i ++) {
      Serial.print(linebuffer[i]);
      Serial.print(" ");
    }
    Serial.println();
#endif
}

void linedisable(void) {
  digitalWrite(COL_OE, false);
}

void lineenable(void) {
  digitalWrite(COL_OE, true);
}

void rowshift(boolean v) {
  if (v)
    digitalWrite(ROW_D, 1);
  else 
    digitalWrite(ROW_D, 0);
  digitalWrite(ROW_CP, 1);
  digitalWrite(ROW_CP, 0);
}  

void fillLineShift(void) {
#ifdef XDEBUG
    Serial.print("fillLine:");
    for(byte i=0; i < 16; i ++) {
      Serial.print(linebuffer[i]);
      Serial.print(" ");
    }
    Serial.println();
#endif
  volatile byte *lend = &linebuffer[ XRES * 2 ];
  do{
    byte d;
    for(byte i=0; i < 8; i++) {
      lend--;
      d <<= 1;
      if (*lend) {
        d|=1;
        (*lend)--;
      }
    }
#ifndef DEBUG
    SPI.transfer(d);
#endif
  } while( lend != linebuffer );
}

void activateLine(boolean stat=true) {
#ifndef DEBUG
  digitalWrite(COL_ST, stat);
#endif
}

void callback() {
  activateLine(false);
  fillLineShift();
  linedisable();
  activateLine();
  if (!lineiters) rowshift(line != 0);
  lineenable();
  
  lineiters++;
  if (lineiters == 3) {
    lineiters = 0;
    line++; line &= 7;
    calculateBuffer(line);
  }

}

void setup() {
  pinMode(13, OUTPUT);
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
  
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(LSBFIRST);
  for(int i = 0; i < 8; i++) {
    SPI.transfer(0);
    rowshift(true);
  }
  digitalWrite(ROW_OE, true);

  digitalWrite(COL_ST, true);
    SPI.transfer(2+8+32+128);
    SPI.transfer((2+8+32+128)>>1);
  digitalWrite(COL_ST, false);

#ifndef DEBUG
  Timer1.initialize(200);
#else
  Timer1.initialize(100000);
  Serial.begin(19200);
  Serial.println("start");
#endif
  displayTestImage();
  Timer1.attachInterrupt(callback);
}

void loop() {

  
}
