#include "display.h"
#include "display_timer.h"
#include "switch.h"
#include "WiShield/WiShield.h"
#include "rtc/RTClib.h"
#include "RadioEvent.h"
#include "WiShield/pt.h"

extern "C" {
#include "WiShield/timer.h"
}

void handleNetworkErrors();
void handleButtons();
void handleLocks();
PT_THREAD( showEvents(struct pt *pt) );
PT_THREAD( updateTime(struct pt *pt, struct timer *timer));

int main() {

  radioEvent.init();
  display.init();
  RTC.begin();
  setupSwitch();
  uint8_t c=0;

  while(1==1){
	struct pt pt1;
	struct timer timer1;
	PT_INIT(&pt1);

	struct pt pt2;
	PT_INIT(&pt2);

	struct pt pt3;
	struct timer timer3;
	PT_INIT(&pt3);

	radioEvent.updateTime();
	wifi = Wifi();
	wifi.restartStack(); 
	radioEvent.setSystemStart();
	radioEvent.setRequestTime();

	uint8_t online=1;
	while(online==1) {
		online=PT_SCHEDULE(wifi.run(&pt1, &timer1));
		showEvents(&pt2);
		updateTime(&pt3, &timer3);
	}
    }
}

PT_THREAD( showEvents(struct pt *pt) ){
	PT_BEGIN(pt);
	while(1) {
		//handleLocks()
		handleButtons();
		handleNetworkErrors();
		display.switchBuffers();
		display.clearBuffer();
		radioEvent.showCurrentEvent(wifi.connected());	  
		display.flush();
		PT_WAIT_UNTIL(pt, (display.flushing()==false));
		//radioEvent.updateTime();
	}
	PT_END(pt);
}

void handleNetworkErrors(){
	if(wifi.connected()){
		if(radioEvent.unixTime > radioEvent.requestTime+240){
			wifi.reconnect();
			radioEvent.reconnects++;
		}
		if(radioEvent.unixTime > radioEvent.requestTime+360){
			wifi.restartStack();
			radioEvent.reconnects++;
		}
	}else{

		if(radioEvent.unixTime > radioEvent.requestTime+90){
			void (*fptr)(void);
			fptr = (void (*)(void))0x0000;
			fptr();
		}
	}
}

void handleButtons(){
	if (buttonPressed(sw::left))  {
		wifi.reconnect();
		radioEvent.setRequestTime();
		radioEvent.reconnects++;
	}
	if (buttonPressed(sw::right)) {
		wifi.restartStack();
		radioEvent.setRequestTime();
		radioEvent.reconnects++;
	}
	if (buttonPressed(sw::up))    display.increaseBrightness();
	if (buttonPressed(sw::down))  display.decreaseBrightness();
	if ( buttonPressed(sw::center)){
		void (*fptr)(void);
		fptr = (void (*)(void))0x0000;
		fptr();
	}
}


PT_THREAD( updateTime(struct pt *pt, struct timer *timer)){
	PT_BEGIN(pt);
	while(1) {
		radioEvent.updateTime();
		timer_set(timer, 100);
		PT_WAIT_UNTIL(pt, timer_expired(timer));
	}
	PT_END(pt);
}



void handleLocks(){
	/*
		if (
			(radioEvent.clockLock==1)
			|| (radioEvent.messageLock==1)
			|| (radioEvent.netLock==1)
			|| (display.lock==1)
		){
			display.waitUntilFlushed();
			display.clearBuffer();
			display.drawStringCenter(ERROR);
			display.flush();
		}
	*/	
}

