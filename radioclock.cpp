#include <stdint.h>
#include "display.h"
#include "display_timer.h"
#include "wiring.h"
#include "switch.h"
#include "WiShield/WiShield.h"
#include <stdio.h>
//#include <avr/interrupt.h>
//#include <avr/wdt.h>
#include "rtc/RTClib.h"
//#include "menusystem/MenuSystem.h"
#include "RadioEvent.h"
//#include "cosine.h"

//int8_t bcnt = 0;
//void setupMenu(menu::MenuSystem &m);

/*
void colorTest(DateTime d){
	  //display.colorBar(rainbow);
  	  volatile uint8_t *drawBuffer = display.getDrawBuffer();
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
}
*/

/*
void drawSinus(){
	  // show sinus
	  double t=getMilliSeconds()/10;
	  for (double x=0; x<24;x+=0.5){
	    //double u=t/50.0+x*6.26/20.0;
	    //display.plot(x+8,round(3.5+ 3.5*sin(u)), 0);
	    display.plot(x+8,round(3.5+ 3.5*sin(t/50.0)*sin(x*(sin(t/60.0)+1.5)*3.14/12)), 1,0);
	  }
}
*/

/*

void drawSecondsBar(DateTime d){
	  //show seconds bar
          for (int y=0; y<=3; y++){
	    display.plot(d.second(),y   ,0,1);
	    display.plot(d.second(),7-y ,0,1);
	  }
}
*/

/*

void writeUnixtime(DateTime d){
	  sprintf(string, "%lu", d.unixtime());
	  display.drawString(0, string);
}
*/

/*

void scrollText(){
	  display.scrollString("test");
}
*/

/*

void showUpTime(DateTime d, uint32_t second, uint32_t c){
	  if(second!=d.second()) c++;
	  second=d.second();
	  sprintf(string, "%lu", c);
	  display.drawString(32, string);
 	  //show seconds
	  sprintf(string, "%02d", second);
	  display.drawString(53, string);

}

void scrollTest{
	display.waitUntilFlushed();
	display.clearBuffer();
	char string []="Das Ist allesNurEinGanzSchönLangerTest.";
	DateTime d = RTC.now();
	if(d.second()<30){
	display.setBorder(10, 60);
	display.scrollString(string);
	if (!WiFi.connected()){
		display.rotateStar();
	}
	display.flush();
}

*/


int main() {
/*
  Menu m("");
  Menu m_bssid("bssid"); m.addChild( m_bssid );
  Menu m_password("password"); m.addChild( m_password );
  Menu m_date_time("Date & Time"); m.addChild( m_date_time );
  Menu m_date("date"); m_date_time.addChild( m_date );
  Menu m_time("time"); m_date_time.addChild( m_time );
  Item *menu = &m;
*/

  radioEvent.init();
  display.begin();
  RTC.begin();
  setupSwitch();

/*
  //uint8_t rainbow[] = {1,2,3,2+4,1+8,12,8,4};
  //long int num = 0;
  //uint32_t u = d.unixtime();
  //uint16_t t = 0;
  //uint8_t colorbars[8];
  //uint16_t second=0;
  //uint16_t c=0;
  //display.colorBar(textColor);  
  //display.clearBuffer();
*/
  while(1==1){
	struct pt p;
	PT_INIT(&p);
	while(PT_SCHEDULE(WiFi.run(&p))) {
		if (buttonPressed(sw::up)) display.increaseBrightness();
		if (buttonPressed(sw::down)) display.decreaseBrightness();
		if (buttonPressed(sw::center)){
			void (*fptr)(void);
			fptr = (void (*)(void))0x0000;
			fptr();
		}
		radioEvent.showCurrentEvent(WiFi.connected());	  
		//scrollTest();

	}
    }
}

/*
		//}else{
			//display.drawString(0, string);
		//}
		//display.setTextColor(RED);

		  //clear buffer
		  //volatile uint8_t *drawBuffer = display.getDrawBuffer();
		  //for(uint8_t i=0;i < XRES * 2;i+=8) drawBuffer[i]=0;
		  //for(uint8_t i=0;i < XRES * 2;i+=8) drawBuffer[i+4*XRES]=0;

		  //menu = menu->interact( getButton() );
		  //menu->repaint();

		  if (buttonPressed(sw::right)) num++;
		  if (buttonPressed(sw::left)) num--;
		  if (buttonPressed(sw::up)) num*=2;
		  if (buttonPressed(sw::down)) num/=2;
		  if (buttonPressed(sw::center)) num=0;

*/
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



