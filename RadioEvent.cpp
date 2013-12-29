#include "display.h"
#include "display_timer.h"
#include "WiShield/socketapp.h"
#include "rtc/RTClib.h"
#include "RadioEvent.h"
#include "WiShield/WiShield.h"
//#include <stdlib.h>

RadioEvent::RadioEvent():messageStart(0), messageDuration(0), videoStart(0), videoDuration(0){
	cycles=0;
	systemStart=0;
	requestTime=0;
	reconnects=0;
	netLock=0;
	clockLock=0;
	messageLock=0;
	enableVideo();
}

void RadioEvent::updateTime(){
	clockLock=1;
	//if((cycles%108)!=0)return;
	DateTime datetime=NULL;
	//ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		datetime=RTC.now();
	//}
	clockLock=0;
	unixTime=datetime.unixtime();
	hour=datetime.hour();
	minute=datetime.minute();
	second=datetime.second();
}

void RadioEvent::init(void){
	for (uint8_t i=0;i<MAX_EVENTS;i++){
		setEventStart(i,0);
		if(i<MAX_EVENTS-1)setEventEnd(i,0);
		if(i<MAX_EVENT_TITLES)memset(eventTitles[i], 0x00, TITLE_LENGTH);
	}
	clearMessage();
}

void RadioEvent::setEventTitle(const int8_t index, const char *name){
	if ((index<0)||(index>=MAX_EVENT_TITLES))return;
	memset(eventTitles[index], 0x00, TITLE_LENGTH);
	strncpy(eventTitles[index], name, TITLE_LENGTH);
	eventTitles[index][TITLE_LENGTH-1]=0;
}

void RadioEvent::showProgressBar(const uint8_t eventIndex){
	uint32_t a=getEventStart(eventIndex);
	uint32_t b=getEventEnd(eventIndex);
	if(a==b)return;
	if((a<unixTime) and (unixTime<b)){
		int32_t progress=64*(unixTime-a)/(b-a);
		if(progress<0)return;
		if(progress>63)return;
		display.showProgressBar(progress);
	}
}


void RadioEvent::copyBuffer(const char* data){
	//ATOMIC_BLOCK(ATOMIC_FORCEON) {
		memcpy(message, data, FRAME_SIZE);
	//}
}

void RadioEvent::showCurrentEvent(const bool isWifiConnected){
	uint32_t duration=-1;
	int8_t eventIndex=0;

	bool isEvent   = false;
	bool isEventRunning = false;
	bool isMessage = false;
	bool isVideo   = false;
	cycles++;

	if(!isEvent){
		eventIndex=getRunningEvent();
		if(eventIndex!=-1){
			duration=getDifference(unixTime, getEventEnd(eventIndex));
			isEvent=true;
			isEventRunning=true;
			display.setColor(RED);
		}
	}
	if(!isEvent){
		eventIndex=getNextEvent();
		if(eventIndex!=-1){
			//show not running yet
			duration=getDifference(unixTime, getEventStart(eventIndex));
			isEvent=true;
			display.setColor(GREEN);
		}
	}

	//show message if no event or event does not end within 60 seconds
	if(unixTime < messageStart+messageDuration) isMessage = true;
	if(unixTime < videoStart  +videoDuration)   isVideo   = true;


	if( isEvent and (duration<600) ){
		//blink background 10 seconds before start
		if( (duration&1) and (duration<=BLINK_TIME) ){
			display.blink();
		}
		showEvent(eventIndex, duration);
	} else if (isMessage){
		//message[MESSAGE_LENGTH-1]=0;
		display.setColor(YELLOW);
		display.resetBorder();
		//if(radioEvent.messageLock==0){
			if(messageScroll==true){
				display.scrollString(message);
			}else{
				display.drawStringCenter(message);
			}
		//}
		//duration=messageDuration;
	} else if (isVideo){
		enableVideo();
		if(
			isVideoEnabled()
//			and(videoLock==0)
		) display.copyBuffer(message);
		display.setColor(YELLOW);
		display.resetBorder();
		display.printDate(15);
	} else if( isEvent and ((((duration+14)/15)&1)==0) ){
		showEvent(eventIndex, duration);
	}else{
		display.setColor(YELLOW);
		display.resetBorder();
		display.printDate(15);
	}
	if ((isEventRunning) and (duration>BLINK_TIME))showProgressBar(eventIndex);

	if(!isWifiConnected){
		display.setColor(RED);
		if((second&1)==0)display.plot(63,7);
	}
}

void RadioEvent::showEvent(const int8_t index, const uint32_t duration){
	if (index<0)return;
	uint8_t days    = getDays(duration);
	uint8_t hours   = getHours(duration);
	uint8_t minutes = getMinutes(duration);
	uint8_t seconds = getSeconds(duration);

	display.resetBorder();

	uint8_t xpos=0;
	xpos=display.printDuration(xpos, days, hours, minutes, seconds);
	xpos+=2;

	if ((index<MAX_EVENT_TITLES) and (duration>BLINK_TIME)){
		display.resetBorder();
		int16_t length=display.getStringSize(getEventTitle(index));

		if(length>XRES-xpos){
			display.setBorder(xpos,XRES-1);
			uint8_t speed=display.scrollSpeed;
			int16_t range=(XRES-1-xpos+length);
			if (speed==0)speed=1;
			if (range==0)range=1;
			int16_t scrollPos=XRES-1-( (cycles/speed) % range );
			xpos=display.drawString(scrollPos, getEventTitle(index));
		}else{
			xpos=display.drawString(xpos, getEventTitle(index));
		}
	}
}

int8_t RadioEvent::getRunningEvent(){
	for (uint8_t index=0;index<MAX_EVENTS-1;index++){
		if(eventExists(index) and (getEventStart(index)<=unixTime) and (unixTime<=getEventEnd(index))){
			return index;
		}
	}
	return -1;
}

int8_t RadioEvent::getNextEvent(){
	for (uint8_t index=0;index<MAX_EVENTS;index++){
		if(eventExists(index) and (getEventStart(index)>=unixTime)){
			return index;
		}
	}
	return -1;
}

void RadioEvent::setEvent(const int8_t index, const uint32_t start, const uint32_t end, const char *name){	
	if (index<0) return;
	setEventStart(index,start);
	setEventEnd(index,end);
	if (index<MAX_EVENT_TITLES)setEventTitle(index,name);
}

void RadioEvent::setMessage(const uint8_t duration, const bool scroll, const char *text){
	messageStart=unixTime+1;
	messageScroll=scroll;
	messageDuration=duration;
	//memcpy(message,text,130);
	uint16_t min=MESSAGE_LENGTH;
	if(SOCKET_BUFFER_LENGTH<min)min=SOCKET_BUFFER_LENGTH;
	min--;
	strncpy(message, text , min);
	/*
//	uint8_t i=0;
	while((text[i]>30) and (i<min)){
		message[i+1]=0;
		message[i]=text[i];
		i++;
	}
	*/
}

void RadioEvent::setVideoRequest(const uint16_t duration){
	videoStart=unixTime;
	videoDuration=duration;
}

RadioEvent radioEvent;
