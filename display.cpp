#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "display.h"
#include "display_timer.h"
#include "wiring.h"
#include "character.h"
#include "cosine.h"
#include "myspi.h"
#include "bits.h"
#include "switch.h"

#define redMask 0b01010101
#define greenMask (redMask<<1)
#define orangeMask (redMask|greenMask)

#define bin(a,b,c,d,e,f,g,h) ((a<<7)|(b<<6)|(c<<5)|(d<<4)|(e<<3)|(f<<2)|(g<<1)|h)
#define round(x) int(x+0.5)

#define COL_ST 3
#define COL_CLK 4
#define BRIGHT 2*XRES

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

volatile uint8_t scrollSpeed=70;

volatile bool waitClear=true;
extern volatile bool DEBUG;

const uint8_t DisplayTimerCycles[] = { 8, 20, 80};


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

/*
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
*/
/*
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
*/
/*
void LEDDisplay::setTextColor(uint8_t color) { // higher bits = background, lower bits = foreground
  for(int8_t i=3; i>=1; i--) {
    volatile uint8_t *lookupbuffer = drawBuffer;
    volatile uint8_t *fillbuffer = &(drawBuffer[XRES*2*(i-1)]);
    for(uint8_t y=0; y < 8; y++) {
      uint8_t fgred = 0, fggreen =  0, bgred = 0, bggreen = 0;
      if (( color       & 3) == i) fgred   = 0xff;
      if (((color >> 2) & 3) == i) fggreen = 0xff;
      if (((color >> 4) & 3) == i) bgred   = 0xff;
      if ( (color >> 6)      == i) bggreen = 0xff;
      uint8_t x2 = XRES>>3;
      for(uint8_t x=0; x < XRES>>3; x++) {
        uint8_t val = lookupbuffer[x];
	fillbuffer[x]  = (val & fgred)   | ((~val) & bgred);
	fillbuffer[x2] = (val & fggreen) | ((~val) & bggreen);
	x2++;
      }
      lookupbuffer+=XRES>>2;
      fillbuffer  +=XRES>>2;
    }
  }
}
*/

void LEDDisplay::clearBuffer() {
	memset((uint8_t*)drawBuffer, 0x00, MAX_FRAMES*2*XRES);
}

int16_t LEDDisplay::drawChar(char c, int16_t pos) {
	if(c==' ')return pos+2;

	uint8_t charIdx = 127 - 32;
	if (c >= 32 && c <= 127) charIdx = c - 32;

	uint8_t charByte=0;
	uint8_t charSpaceLeft=0;
	uint8_t charSpaceRight=0;
	if(c=='1'){
		charSpaceLeft=1;
	}else{
		//overlay all y rows
		for(uint8_t y=0;y<8;y++) charByte |= charset(charIdx, y);
		//remove space left of character
		if(!(charByte & bin(1,1,1,0,0,0,0,0)))charSpaceLeft=3;
		else if(!(charByte & bin(1,1,0,0,0,0,0,0)))charSpaceLeft=2;
		else if(!(charByte & bin(1,0,0,0,0,0,0,0)))charSpaceLeft=1;
		//remove space right of character
		if(!(charByte & bin(0,0,1,1,1,0,0,0)))charSpaceRight=3;
		else if(!(charByte & bin(0,0,0,1,1,0,0,0)))charSpaceRight=2;
		else if(!(charByte & bin(0,0,0,0,1,0,0,0)))charSpaceRight=1;
	}    
	pos -= charSpaceLeft;
	if (	   
		   (pos < leftBorder-6+charSpaceRight) 
		|| (pos > rightBorder)
	){
		return pos +6-charSpaceRight;
	}
	uint8_t r=0;
	uint8_t g=0;
	if(textColor==RED)r=2;
	if(textColor==GREEN)g=2;
	if(textColor==YELLOW){
		r=2;
		g=2;
	}
	for(uint8_t x=0;x<8;x++) {
		if( (pos+x >= leftBorder) && (pos+x <= rightBorder) ){
			for(uint8_t y=0;y<8;y++) if ( (charset(charIdx, y) & (1<<(8-x))) ) plot(pos+x,y,r,g);
		}
	}
	return pos+6-charSpaceRight;
}

int16_t LEDDisplay::drawString(int16_t pos, const char *string) {
	while((*string!=0) && (pos < XRES) && (pos<rightBorder)) {
		pos=drawChar( *string, pos);
		string++;
	}
	return pos;
}

int8_t LEDDisplay::drawNumber(int8_t xpos, uint8_t number, bool leadingZero, bool align){
	uint8_t c=0;
	if(number>=10){
		c=number/10;
		if((align)&&(c==1)){
			for(uint8_t y=0;y<8;y++)plot(xpos+1,y);
			xpos+=2;
		}else xpos=drawChar(c+'0', xpos);
	}else if(leadingZero){
		xpos=drawChar('0', xpos);
	}
	c=number%10;
	if((align)&&(c==1)){
		for(uint8_t y=0;y<8;y++)plot(xpos+1,y);
		xpos+=2;
	}else xpos=drawChar(c+'0', xpos);
	return xpos;
}

int8_t LEDDisplay::printDate(int8_t pos, uint8_t hour, uint8_t minute, uint8_t second){
	pos=drawNumber(pos, hour,   true, false);
	pos=drawChar(':',pos);
	pos=drawNumber(pos, minute, true, false);
	pos=drawChar(':',pos);
	pos=drawNumber(pos, second, true, false);
	return pos;
}

int8_t LEDDisplay::printDuration(int8_t pos, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second){
	bool valueSet=false;
	if((day>0)){
		pos=drawNumber(pos, day, false, true);
		pos=drawChar('T',pos);
		//pos=drawChar(' ',pos);
		valueSet=true;
	}
	if((valueSet)||(hour>0)){
		pos=drawNumber(pos, hour, true, true);
		pos=drawChar(':',pos);
		valueSet=true;
	}
	if((valueSet)||(minute>0)){
		pos=drawNumber(pos, minute, true, true);
		pos=drawChar(':',pos);
	}
	pos=drawNumber(pos, second, true, false);
	return pos;
}

void LEDDisplay::copyBuffer(uint8_t *line){
	for (uint8_t y=0;y<8;y++){
		for (uint8_t x=0;x<64;x++){
			if ((
				line[(y<<4)+(x>>3)] & (1 << ((63-x)&7) )  
			)>0){
				display.plot(x,y,1,0);
			}
			if ((
				line[(y<<4)+(x>>3)+8] & (1 << ((63-x)&7) )  
			)>0){
				display.plot(x,y,0,1);
			}
		}
	}

	//memcpy((uint8_t*)drawBuffer,line,128);
}

void LEDDisplay::scrollString(const char *string) {
	drawString(XRES-((getMilliSeconds()/scrollSpeed) % (XRES + strlen(string)*5)), string);
}

/*
void LEDDisplay::softScreen(){
	for (int y=0;y<8;y++){
		for (int x=0; x<64;x++){
			uint8_t c=(
				  getRed(x ,y)
				 +getRed(x+1,y)
				 +getRed(x-1,y)
				 +getRed(x  ,y+1)
				 +getRed(x  ,y-1)
			);
			if(c>2)plot(x,y,1,0);

			c=(
				  getGreen(x   ,y)
				 +getGreen(x-1 ,y)
				 +getGreen(x+1 ,y)
				 +getGreen(x   ,y+1)
				 +getGreen(x   ,y-1)
			);
			if(c>2)plot(x,y,0,1);

		}
	}
}
*/
void LEDDisplay::rotateStar(){
	double t=getMilliSeconds()/80;
	t=t*-6.28/64.0;
	double sint1=sin(t);
	double cost1=cos(t);

	double sint2=sin(t+6.28*1.0/12.0);
	double cost2=cos(t+6.28*1.0/12.0);
	double sint3=sin(t+6.28*2.0/12.0);
	double cost3=cos(t+6.28*2.0/12.0);
	double sint4=sin(t+6.28*3.0/12.0);
	double cost4=cos(t+6.28*3.0/12.0);
	double sint5=sin(t+6.28*4.0/12.0);
	double cost5=cos(t+6.28*4.0/12.0);
	double sint6=sin(t+6.28*5.0/12.0);
	double cost6=cos(t+6.28*5.0/12.0);

	for (double i=2.5;i<=3.5;i+=1.0){
		plot(round(3.5+i*sint1), round(3.5+i*cost1), 2, 0);
		plot(round(3.5-i*sint1), round(3.5-i*cost1), 2, 0);

		plot(round(3.5+i*sint2), round(3.5+i*cost2), 1, 0);
		plot(round(3.5-i*sint2), round(3.5-i*cost2), 1, 0);
		plot(round(3.5+i*sint3), round(3.5+i*cost3), 0, 2);
		plot(round(3.5-i*sint3), round(3.5-i*cost3), 0, 2);
		plot(round(3.5+i*sint4), round(3.5+i*cost4), 0, 1);
		plot(round(3.5-i*sint4), round(3.5-i*cost4), 0, 1);
		plot(round(3.5+i*sint5), round(3.5+i*cost5), 2, 2);
		plot(round(3.5-i*sint5), round(3.5-i*cost5), 2, 2);
		plot(round(3.5+i*sint6), round(3.5+i*cost6), 1, 1);
		plot(round(3.5-i*sint6), round(3.5-i*cost6), 1, 1);

	}

}

inline void LEDDisplay::plot(int8_t x, int8_t y){
	uint8_t r=0;
	uint8_t g=0;
	if(textColor==RED)r=2;
	if(textColor==GREEN)g=2;
	if(textColor==YELLOW){
		r=2;
		g=2;
	}
	plot(x,y,r,g);
}

inline void LEDDisplay::plot(int8_t x, int8_t y, int8_t red, int8_t green){
	if((x<0)||(x>63)||(y<0)||(y>7)) return;
	if(red  >0) drawBuffer[brightness +     (y<<4) + (x>>3) ] |= (1 << ((63-x) & 7));
	if(green>0) drawBuffer[brightness + 8 + (y<<4) + (x>>3) ] |= (1 << ((63-x) & 7));

	//if(red  >0) drawBuffer[brightness +     (y<<4) + (x>>3) ] |= (1 << ((63-x) & 7));
	//if(green>0) drawBuffer[brightness + 8 + (y<<4) + (x>>3) ] |= (1 << ((63-x) & 7));
	/*
	if((red==1)||(red==3))   drawBuffer[       (y<<4) +     (x>>3) ] |= 1 << ((63-x) & 7);
	if((red==2)||(red==3))   drawBuffer[brightness+(y<<4) +     (x>>3) ] |= 1 << ((63-x) & 7);

	if((green==1)||(green==3)) drawBuffer[       (y<<4) + 8 + (x>>3) ] |= 1 << ((63-x) & 7);
	if((green==2)||(green==3)) drawBuffer[brightness+(y<<4) + 8 + (x>>3) ] |= 1 << ((63-x) & 7);
	*/
}

uint8_t LEDDisplay::getColor(int8_t x, int8_t y, int8_t colorChanel){
	if(x<0) return 0;
	if(y<0) return 0;
	if(x>63) return 0;
	if(y>7) return 0;
	if ((drawBuffer[brightness+(y<<4) + colorChanel + (x>>3) ] & (1 << ((63-x)&7) ) )>0 ) return 1;
	return 0;
}


uint8_t LEDDisplay::getGreen(int8_t x, int8_t y){
	return getColor(x,y,8);
	if ((drawBuffer[brightness+(y<<4) + 8 + (x>>3) ] & (1 << ((63-x)&7) ) )>0 ) return 1;
	return 0;
}

uint8_t LEDDisplay::getRed(int8_t x, int8_t y){
	return getColor(x,y,8);
	if ((drawBuffer[brightness+(y<<4) + 8 + (x>>3) ] & (1 << ((63-x)&7) ) )>0 ) return 1;
	return 0;
}


inline void setDisplayTimer(uint16_t cycles) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    OCR1A = cycles*100;
  }
  if (!frame) ATOMIC_BLOCK(ATOMIC_FORCEON) {
    current_ticks_lock++;
    current_ticks+=11;
  }
}

void initDisplayTimer(void) {
  TCCR1A = 0;
  TCCR1B = _BV(WGM12);        // set CTC
  
  setDisplayTimer( DisplayTimerCycles[2]);
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

  scrollSpeed	= 70;
  leftBorder	= 0;
  rightBorder	= XRES;
  textColor	= RED;
  brightness	= 0;
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

LEDDisplay display;

