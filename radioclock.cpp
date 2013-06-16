#include <stdint.h>
#include "display.h"
#include "display_timer.h"
#include "wiring.h"
#include "switch.h"
#include "WiShield/WiShield.h"
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "rtc/RTClib.h"
#include "menusystem/MenuSystem.h"


uint16_t t = 0;
uint8_t colorbars[8];
int8_t bcnt = 0;
RTC_DS1307 RTC;

void setupMenu(menu::MenuSystem &m);

int main() {
  menu::MenuSystem m;
  setupMenu(m);
  DISP.begin();
  setupSwitch();
  RTC.begin();

  char mystring[21];
  struct pt p;
  PT_INIT(&p);
  uint8_t red[] = {1,2,3,2+4,1+8,12,8,4};
  long int num = 0;
  DateTime d = RTC.now();
  uint32_t b = d.unixtime();
  while(PT_SCHEDULE(WiFi.run(&p))) {
	  while(DISP.flushing());
	  if (buttonPressed(SW_RIGHT)) num++;
	  if (buttonPressed(SW_LEFT)) num--;
	  if (buttonPressed(SW_UP)) num*=2;
	  if (buttonPressed(SW_DOWN)) num/=2;
	  if (buttonPressed(SW_PUSH)) num=0;
	  DISP.clearBuffer();
	  d = RTC.now();
	  sprintf(mystring, "%ld %ld %02d:%02d:%02d", num, getCurrent_ms()/(d.unixtime()-b), d.hour(), d.minute(), d.second());
	  DISP.drawString(8, mystring);
//	  scrollString(drawBuffer, mystring, getCurrent_ms()/40);
	  DISP.colorBar(red);
/*	  
	  clearBuffer(drawBuffer);
	  scrollString(drawBuffer, "setting up wifi", getCurrent_ticks()/50);
	  colorBar(drawBuffer, red);
	  switchBuffersFlag = 1;
*/

	  volatile uint8_t *drawBuffer = DISP.getDrawBuffer();
	  drawBuffer[0]=0;
	  drawBuffer[14*XRES/8]=0;
	  drawBuffer[3*XRES/8]=0b01111111;
	  drawBuffer[12*XRES/8]=0b11111110;
	  for(uint8_t i=5; i<16; i+=2) {
	  	drawBuffer[i*XRES/8]=0b01000000;
	  	drawBuffer[(i-5)*XRES/8]=0b00000010;
	  }

	  drawBuffer[(16+5)*XRES/8]=0b00111111;
	  drawBuffer[(16+10)*XRES/8]=0b11111100;
	  for(uint8_t i=7; i<16; i+=2) {
	  	drawBuffer[(16+i)*XRES/8]=0b00100000;
	  	drawBuffer[(32+i)*XRES/8]=0b00011111;
	  	drawBuffer[(16+i-7)*XRES/8]=0b00000100;
	  	drawBuffer[(32+i-7)*XRES/8]=0b11111000;
	  }
		
	  DISP.flush();
  }
}

void setupMenu(menu::MenuSystem &m) {
	menu::Menu mm("");
	menu::TextItem mm_mi1("Level 1 - Item 1");
	menu::TextItem mm_mi2("Level 1 - Item 2");
	menu::Menu mu1("Level 1 - Item 3 (Menu)");
	menu::Menu mu1_mi1("Level 2 - Item 1");
	mm.add_item(&mm_mi1);
}
