#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include "display.h"
#include "display_timer.h"
#include "wiring.h"
#include "character.h"
#include "cosine.h"
#include "myspi.h"
#include "bits.h"
#include "switch.h"
#include "RadioEvent.h"

volatile uint8_t  line = 255;
volatile uint8_t  lineBit = 0;
volatile uint32_t current_ticks = 0;
volatile uint8_t  current_ticks_lock;
//volatile uint8_t  *temp;
const uint16_t DisplayTimerCycles[] = {800, 8000};


inline void LedDisableClock() {
	wdt_reset();
	digitalWrite(ROW_OE, false);
}
inline void LedEnableShiftClock(const uint8_t v) {
	digitalWrite(ROW_CP, 0);
	digitalWrite(ROW_D, v);
	digitalWrite(ROW_CP, 1);
	digitalWrite(ROW_OE, true);
}

inline void storeLine(void) {
	digitalWrite(COL_ST, true);
	digitalWrite(COL_ST, false);
}

#define debug(l)
//#define debug(l) plot(l%50, l/50, 0,1)


void LedDisplay::clearBuffer() {
	memset((uint8_t*)drawBuffer, 0x00, MAX_FRAMES*FRAME_SIZE);
}

uint8_t LedDisplay::getCharWidth(const char c){
	if (c < 32 or c > 127) return 6;
	uint8_t width=charsize(c-32);
	if(width>6)width=6;
	if(width<1)width=1;
	return width;
}

uint16_t LedDisplay::getStringSize(const char *string) {
	uint8_t i=0;
	uint16_t size=0;
	volatile uint8_t c=0;
	while((radioEvent.messageLock==0) and (i<100)) {
		c=string[i];
		if (c<32)break;
		size+=getCharWidth(c);
		i++;
	}
	return size;
}

uint8_t LedDisplay::drawChar(const char c, int16_t xpos) {
	if (c < 32 or c > 127) return 6;
	uint8_t charIdx = c - 32;
	uint8_t width=charsize(charIdx);
	if(width>6)width=6;
	if(width<1)width=1;
	if (xpos > rightBorder  )    return width;
	if (xpos < leftBorder-width )return width;
	if(c==' ')                   return width;

	if(xpos>leftBorder and xpos<rightBorder-width){
		uint8_t x=xpos;
		uint16_t addr = brightness + (x>>3);
		for(uint8_t y=0;y<8;y++){
			uint8_t color=charset(charIdx, y);
			if (red==1){
				if(xpos> 0)drawBuffer[addr]  |=color >> (x&7);
				if(xpos<56)drawBuffer[addr+1]|=color << (8-(x&7));
			}
			if(green==1){
				if(xpos> 0)drawBuffer[addr+8]  |=color >> (x&7);
				if(xpos<56)drawBuffer[addr+8+1]|=color << (8-(x&7));
			}
			addr+=16;
		}
	}else{
		for(uint8_t x=0;(x<width) and (xpos+x<=rightBorder);x++){
			if (xpos+x >= leftBorder){
				for(uint8_t y=0;y<8;y++){
					if ((radioEvent.messageLock==0) and ((charset(charIdx, y) & (0x01<<(7-x))))) 
						plot(xpos+x,y);
				}
			}
		}
	}

	return width;
}

int16_t LedDisplay::drawString(volatile int16_t pos, const char *string) {
	uint8_t i=0;
	volatile uint8_t c=0;
	while((pos < XRES) and (pos<rightBorder) and (i<100)) {
		c=string[i];
		if(c<32 or c>127)break;
		pos+=drawChar(c, pos);
		i++;
	}
	return pos;
}

void LedDisplay::drawStringCenter(const char *string){
	uint16_t size=getStringSize(string);
	uint8_t pos=0;
	if (rightBorder<=leftBorder)return;
	uint8_t range=rightBorder-leftBorder;
	if(size<range){
		pos=(1+range-size)>>1;
	}
	drawString(pos, string);
}

uint8_t LedDisplay::drawNumber(const int16_t xpos, const uint8_t number, const bool leadingZero, const bool align){
	uint8_t c=0;
	uint8_t width=0;
	if(number>=100)return width;
	if(number>=10){
		c=number/10;
		if((align) and (c==1)){
			if((xpos>=0) and (xpos<XRES))for(uint8_t y=0;y<YRES-1;y++)plot(xpos+width,y);
			width+=2;
		}else{
			width+=drawChar(c+'0', xpos+width);
		}
		
	}else if(leadingZero){
		width+=drawChar('0', xpos+width);
	}
	c=number%10;
	if((align) and (c==1)){
		if((xpos+width>=0) and (xpos+width<XRES))for(uint8_t y=0;y<YRES-1;y++)plot(xpos+width,y);
		width+=2;
	}else {
		width+=drawChar(c+'0', xpos+width);
	}
	return width;
}

int16_t LedDisplay::printDate(int16_t pos){
	if((radioEvent.hour>23)or(radioEvent.minute>59)or(radioEvent.second>59)) return pos;
	pos+=drawNumber(pos, radioEvent.hour,   true, false);
	pos+=drawChar(':',pos);
	pos+=drawNumber(pos, radioEvent.minute, true, false);
	pos+=drawChar(':',pos);
	pos+=drawNumber(pos, radioEvent.second, true, false);
	return pos;
}

int16_t LedDisplay::printDuration(int16_t pos, uint8_t day, const uint8_t hour, const uint8_t minute, const uint8_t second){
	bool valueSet=false;
	if((day>0)){
		pos+=drawNumber(pos, day, false, true);
		pos+=drawChar('T',pos);
		valueSet=true;
	}
	if((valueSet)or(hour>0)){
		pos+=drawNumber(pos, hour, true, true);
		pos+=drawChar(':',pos);
		valueSet=true;
	}
	if((valueSet)or(minute>0)){
		pos+=drawNumber(pos, minute, true, true);
		pos+=drawChar(':',pos);
	}
	pos+=drawNumber(pos, second, true, false);
	return pos;
}

void LedDisplay::copyBuffer(const char *line){
	memcpy((char*)drawBuffer,line, FRAME_SIZE);
}

void LedDisplay::scrollString(const char *string) {
	uint16_t size=getStringSize(string);
	if(size>130*5)size=130*5;
	uint16_t range=(XRES + size);
	if(scrollSpeed==0)scrollSpeed=1;
	if(range==0)range=1;
	int16_t pos=XRES-((radioEvent.cycles/scrollSpeed) % range);
	
	drawString(pos, string);
}

void LedDisplay::showProgressBar(uint8_t progress){
	setColor(GREEN);
	if((progress>0) and (progress<64)){
		for (uint8_t x=0;x<progress;x++)plot(x,7);
		uint16_t addr      = brightness + (7<<4) +8;
		uint8_t xstart=0;
		for (uint8_t x=0;x<progress>>3;x++){
			drawBuffer[addr+x] = 255;
			xstart+=8;
		}
		for (uint8_t x=0;x<(progress&7);x++)plot(xstart+x,7);
	}
}


void LedDisplay::rotateStar(){
	float t=getMilliSeconds()/80;
	t=t*-6.28/64.0;
	float sint1=sin(t);
	float cost1=cos(t);

	float sint2=sin(t+6.28*1.0/12.0);
	float cost2=cos(t+6.28*1.0/12.0);
	float sint3=sin(t+6.28*2.0/12.0);
	float cost3=cos(t+6.28*2.0/12.0);
	float sint4=sin(t+6.28*3.0/12.0);
	float cost4=cos(t+6.28*3.0/12.0);
	float sint5=sin(t+6.28*4.0/12.0);
	float cost5=cos(t+6.28*4.0/12.0);
	float sint6=sin(t+6.28*5.0/12.0);
	float cost6=cos(t+6.28*5.0/12.0);

	for (float i=2.5;i<=3.5;i+=1.0){
		float radius=i;
		//if(radius==2.5)radius=2.5*sint1;
		setColor(RED);
		plot(round(3.5+radius*sint1), round(3.5+radius*cost1));
		plot(round(3.5-radius*sint1), round(3.5-radius*cost1));
		plot(round(3.5+radius*sint2), round(3.5+radius*cost2));
		plot(round(3.5-radius*sint2), round(3.5-radius*cost2));
		setColor(GREEN);
		plot(round(3.5+radius*sint3), round(3.5+radius*cost3));
		plot(round(3.5-radius*sint3), round(3.5-radius*cost3));
		plot(round(3.5+radius*sint4), round(3.5+radius*cost4));
		plot(round(3.5-radius*sint4), round(3.5-radius*cost4));
		setColor(YELLOW);
		plot(round(3.5+radius*sint5), round(3.5+radius*cost5));
		plot(round(3.5-radius*sint5), round(3.5-radius*cost5));
		plot(round(3.5+radius*sint6), round(3.5+radius*cost6));
		plot(round(3.5-radius*sint6), round(3.5-radius*cost6));

	}

}
void LedDisplay::blink(){
	setColor(RED);
	for(uint8_t y=0;y <= 7;y++){
		for(uint8_t x=10;x <64 ;x+=2){
			plot(x,y);
		}
	}
}

void LedDisplay::plot(const int16_t x, const int16_t y){
	if((x<0)||(x>XRES-1)||(y<0)||(y>YRES-1)) return;

	uint16_t addr      = brightness + (y<<4) + (x>>3);
	uint8_t  colorBits = 0x01 << ((XRES-1-x) & 7);
	if(red==1)   drawBuffer[addr   ] |= colorBits;
	if(green==1) drawBuffer[addr +8] |= colorBits;
}


inline void setDisplayTimer(const uint16_t cycles) {
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		OCR1A = cycles;
		if(display.frame==0){
			current_ticks_lock++;
			current_ticks+=5;
		}
	}
}

void initDisplayTimer(void) {
	TCCR1A = 0;
	TCCR1B = _BV(WGM12);        // set CTC

	setDisplayTimer( DisplayTimerCycles[0]);
	TIMSK1 = _BV(OCIE1A);// sets the timer 1 output compare interrupt enable bit

	// Set Clock Prescaler to NO Prescaler
	uint8_t clockSelectBits = _BV(CS10);
	TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
	TCCR1B |= clockSelectBits;   
}


inline void fillLineShift(void) {
	volatile uint8_t *linebuffer = &(display.displayBuffer[ display.frame * (FRAME_SIZE) + line * LINE_SIZE]);
	SPItransferBufferReverse(linebuffer, LINE_SIZE);
}


ISR(TIMER1_COMPA_vect) {
	display.lock=1;
	if (!display.waitClear) {
		LedDisableClock();
		display.waitClear = true;
	} else{
		if (radioEvent.clockLock==1)return;
		if (SPIfinished()) {
			storeLine();
			LedEnableShiftClock(line);
			updateButton(lineBit);
			if (line==0) setDisplayTimer( DisplayTimerCycles[display.frame] );
			line++;
			lineBit<<=1;
			if (line >= 8) {
				line = 0;
				lineBit = 1;
				display.frame++; 
				if ( display.frame >= MAX_FRAMES ) {
					display.frame = 0;
					display.switchBuffer = false;
				}
			}
			fillLineShift();
			display.waitClear = false;
		}
	}
	display.lock=0;
}

void LedDisplay::init(void){
	frame = 0;
	lock=0;
	waitClear=true;
	switchBuffer=false;
	displayBuffer = displayBuff;
	drawBuffer    = drawBuff;

	scrollSpeed	= 5;
	leftBorder	= 0;
	rightBorder	= XRES;
	red		= 1;
	green		= 0;
	brightness	= FRAME_SIZE;

	pinMode(ROW_OE, OUTPUT);
	LedEnableShiftClock(true);
	LedDisableClock();
	pinMode(COL_ST, OUTPUT);
	digitalWrite(COL_ST, false);
	pinMode(ROW_D, OUTPUT);
	pinMode(ROW_CP, OUTPUT);
	pinMode(ROW_ST, OUTPUT);
	digitalWrite(ROW_ST, true);

	SPIsetup();
	for(int i = 0; i < 8; i++) SPItransfer(0);
	initDisplayTimer();
	wdt_enable( WDTO_15MS );
}

LedDisplay display;

