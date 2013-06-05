#include <stdint.h>
#include "display.h"
#include "scroller.h"
#include "wiring.h"
#include "switch.h"
#include "WiShield/WiShield.h"
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "rtc/RTClib.h"


uint16_t t = 0;
uint8_t colorbars[8];
int8_t bcnt = 0;
RTC_DS1307 RTC;

volatile bool DEBUG=false;

int main() {
  setupDisplay();
  setupSwitch();
  RTC.begin();

  uint16_t stop=0;

  char mystring[21];
  struct pt p;
  PT_INIT(&p);
  uint8_t red[] = {1,2,3,2+4,1+8,12,8,4};
  long int num = 0;
  DateTime d;
  while(PT_SCHEDULE(WiFi_init(&p))) {
	  while(switchBuffersFlag);
	  if (buttonPressed(SW_RIGHT)) num++;
	  if (buttonPressed(SW_LEFT)) num--;
	  if (buttonPressed(SW_UP)) num*=2;
	  if (buttonPressed(SW_DOWN)) num/=2;
	  if (buttonPressed(SW_PUSH)) num=0;
	  clearBuffer(drawBuffer);
	  d = RTC.now();
	  sprintf(mystring, "%ld %ld %02d:%02d:%02d", num, getCurrent_ms()/1000, d.hour(), d.minute(), d.second());
// 	  sprintf(mystring, "%ld %d", num, RTC.isrunning() );
//	  drawString(drawBuffer, 0, mystring);
	  scrollString(drawBuffer, mystring, getCurrent_ms()/40);
	  colorBar(drawBuffer, red);
	  switchBuffersFlag = 1;
/*	  
	  clearBuffer(drawBuffer);
	  scrollString(drawBuffer, "setting up wifi", getCurrent_ticks()/50);
	  colorBar(drawBuffer, red);
	  switchBuffersFlag = 1;
*/
  }

  PT_INIT(&p);
  while(PT_SCHEDULE(WiFi_run(&p))) {
	  if (DEBUG) continue;
	  while(switchBuffersFlag);
	  clearBuffer(drawBuffer);
	  if (!stop)  {
	  }
	  else stop--;
/*
	  char bChar=0;

	  if (switchPressed(SW_F_UP)) bChar = 'U';
	  if (switchPressed(SW_F_DOWN)) bChar = 'D';
	  if (switchPressed(SW_F_LEFT)) bChar = 'L';
	  if (switchPressed(SW_F_RIGHT)) bChar = 'R';
	  if (switchPressed(SW_F_PUSH)) bChar = 'P';
	  if (bChar =='L') { *mystring=(--bcnt)+'0'; stop = 200; }
	  else if (bChar =='R') { *mystring=(++bcnt)+'0'; stop = 200; }
	  else if (bChar =='P') { *mystring=(bcnt)+'0'; stop = 200; }
	  else if (bChar) { *mystring = bChar; stop = 200; }
*/

//	  scrollString(drawBuffer, mystring, getCurrent_ticks()/50);
	  drawString(drawBuffer, 0, mystring);
	  getCopperBars( colorbars, t/4 );
	  colorBar(drawBuffer, colorbars);
	  switchBuffersFlag = 1;
	  t++;
  }
}
