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
#include "cosine.h"

#define round(x) int(x+0.5)

uint16_t t = 0;
uint8_t colorbars[8];
int8_t bcnt = 0;

//void setupMenu(menu::MenuSystem &m);

int main() {

  Menu m("");
  Menu m_bssid("bssid"); m.addChild( m_bssid );
  Menu m_password("password"); m.addChild( m_password );
  Menu m_date_time("Date & Time"); m.addChild( m_date_time );
  Menu m_date("date"); m_date_time.addChild( m_date );
  Menu m_time("time"); m_date_time.addChild( m_time );
  Item *menu = &m;

  DISP.begin();
  setupSwitch();
  RTC.begin();

  char mystring[21];
  uint8_t rainbow[] = {1,2,3,2+4,1+8,12,8,4};
  //long int num = 0;
  DateTime d = RTC.now();
  //uint32_t b = d.unixtime();
  uint32_t second=0;
  uint32_t c=0;

  struct pt p;
  PT_INIT(&p);

  while(PT_SCHEDULE(WiFi.run(&p))) {

	  DISP.waitUntilFlushed();
	  
	  DISP.clearBuffer();

	  d = RTC.now();

	  //menu = menu->interact( getButton() );
	  //menu->repaint();
/*
	  if (buttonPressed(sw::right)) num++;
	  if (buttonPressed(sw::left)) num--;
	  if (buttonPressed(sw::up)) num*=2;
	  if (buttonPressed(sw::down)) num/=2;
	  if (buttonPressed(sw::center)) num=0;
	  sprintf(mystring, "%ld %ld %02d:%02d:%02d", num, getCurrent_ms()/(d.unixtime()-b), d.hour(), d.minute(), d.second());
	  DISP.drawString(8, mystring);
//	  scrollString(drawBuffer, mystring, getCurrent_ms()/40);
*/
/*	  
	  clearBuffer(drawBuffer);
	  scrollString(drawBuffer, "setting up wifi", getCurrent_ticks()/50);
	  colorBar(drawBuffer, red);
	  switchBuffersFlag = 1;
*/

	  //DISP.scrollString("wifi", getCurrent_ms()/100);

	  // show uptime
	
	  if(second!=d.second()) c++;
	  second=d.second();
	  sprintf(mystring, "%lu", c);
	  DISP.drawString(32, mystring);
 	  //show seconds
	  sprintf(mystring, "%02d", second);
	  DISP.drawString(53, mystring);

	
          
	  // show sinus
	  double t=getCurrent_ms()/10;
	  for (int x=10; x<=32;x++){
	    double u=t/50.0+x*6.26/20.0;
	    DISP.plot(x,round(4+ 4.0*sin(u)), 1,0);
	    DISP.plot(x,round(4+ 4.0*cos(u)), 0,1);
	  }
	  //show seconds bar
          for (int y=0; y<=2; y++){
	    DISP.plot(d.second(),y ,1,0);
	    DISP.plot(d.second(),7-y ,1,0);
	  }
	

	  //show unixtime
	  //sprintf(mystring, "%lu", d.unixtime());
	  //DISP.drawString(0, mystring);

	  //DISP.scrollString("test", getCurrent_ms()/20);
	  
	   
	  if (WiFi.connected()){
		  DISP.colorBar(rainbow);
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
	 }else{
		double t=getCurrent_ms()/40;
		t=t*-6.28/64.0;
		double sint=sin(t);
		double cost=cos(t);
		//DISP.plot(round(4.5+4.0*sint),round(4.5+4.0*cost), 1,0);
		DISP.plot(round(4+3.0*sint),round(4+3.0*cost), 1,0);
		DISP.plot(round(4+2.0*sint),round(4+2.0*cost), 1,0);
		DISP.plot(round(4+1.0*sint),round(4+1.0*cost), 1,0);
		DISP.colorBar(rainbow);
	 }
	
	/*
	  if (WiFi.connected()){
		  DISP.drawString(8, "connected");
          }else{
	  	  DISP.drawString(8, "connect");
	  }
	*/
	
	 DISP.flush();
  }
}

/*
void setupMenu(menu::MenuSystem &m) {
	menu::Menu mm("");
	menu::TextItem mm_mi1("Level 1 - Item 1");
	menu::TextItem mm_mi2("Level 1 - Item 2");
	menu::Menu mu1("Level 1 - Item 3 (Menu)");
	menu::Menu mu1_mi1("Level 2 - Item 1");
	mm.add_item(&mm_mi1);
}
*/



