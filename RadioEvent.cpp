#include "display.h"
#include "display_timer.h"
#include "WiShield/socketapp.h"
#include "rtc/RTClib.h"
#include "RadioEvent.h"
//#include <stdlib.h>


void RadioEvent::init(void){
	for (uint8_t i=0;i<MAX_EVENTS;i++){
		setEventStart(i,0);
		if(i<MAX_EVENTS-1)setEventEnd(i,0);
		if(i<2)memset(eventTitles[i], 0x00, NAME_LENGTH);
	}
	messageStart=0;
	messageDuration=0;
	clearMessage();
	enableVideo();
}

//print to LED Display at position xpos if toDisplay=true
//print to string dest at position xpos if toDisplay=false
int16_t RadioEvent::printEventTitle(int8_t index, bool toDisplay, int16_t xpos, bool toString, char *dest){
	if ((index<0)||(index>1))return xpos;

	unsigned char *title=eventTitles[index];
	//memset(dest, 0x00, LINE_LENGTH-8);
	uint8_t i=0;
	uint8_t j=0;
	uint8_t pos_a=0;
	uint8_t pos_b=3;
	uint8_t char_a=title[i];
	uint8_t char_b=0;
	while((i<NAME_LENGTH-1) && (j<LINE_LENGTH-2) && (char_a!=0)){
		if((char_a & (1<<(7-pos_a)))) char_b |= (1 << (7-pos_b));
		pos_a++;
		pos_b++;
		if(pos_a>=8){
			pos_a=0;
			i++;
			char_a=title[i];
		}
		if(pos_b>=8){
			if(char_b==1){
				char_b=' ';
			}else{
				char_b+='A'-2;
			}
			if(toDisplay){
				xpos=display.drawChar(char_b,xpos);
			}else{
				dest[xpos+j]=char_b;
			}
			pos_b=3;
			char_b=0;
			j++;
		}
	}
	if(!toDisplay){
		xpos+=j;
		dest[xpos+1]=0;
	}
	return xpos;
	//return dest;
}

void RadioEvent::setEventTitle(int8_t index, unsigned char *name){
	if ((index<0)||(index>1))return;
	unsigned char *title=eventTitles[index];
	memset(title, 0x00, NAME_LENGTH);
	uint8_t i=0;
	uint8_t j=0;
	uint8_t pos_a=3;
	uint8_t pos_b=0;
	uint8_t char_a=name[i]-'A'+2;
	if(name[i]=='\n')return;
	if(name[i]==' ')char_a=1;
	title[j]=0;
	while((i<SOCKET_BUFFER_LENGTH) && (j<NAME_LENGTH-1) && (name[i]!=0)){
		if (char_a & (1<<(7-pos_a))) title[j] |= (1 << (7-pos_b));
		pos_a++;
		pos_b++;
		if(pos_a>=8){
			pos_a=3;
			i++;
			if(name[i]==' ')char_a=1;
			else if ((name[i]<'A')||(name[i]>'Z'))char_a=1;
			else char_a=name[i]-'A'+2;
		}
		if(pos_b>=8){
			pos_b=0;
			j++;
			title[j]=0;
		}
	}
	title[j]=0;
}

void RadioEvent::showProgressBar(uint8_t eventIndex){
	DateTime datetime=RTC.now();
	uint8_t progress=64*(datetime.unixtime()-getEventStart(eventIndex))/(getEventEnd(eventIndex)-getEventStart(eventIndex));
	for (uint8_t x=0;x<progress;x++){
		display.plot(x,7,0,1);
	}
}

void RadioEvent::blink(){
	for(uint8_t y=0;y <= 7;y++){
		for(uint8_t x=16;x <=64 ;x++){
			display.plot(x,y,1,0);
		}
	}
}

void RadioEvent::showCurrentEvent(bool isWifiConnected){
	uint32_t diff=-1;
	int8_t eventIndex=0;
	DateTime datetime=RTC.now();
	display.setTextColor(RED);

	bool isEvent   = false;
	bool isEventRunning = false;
	bool isMessage = false;
	bool isVideo   = false;

	if(!isEvent){
		eventIndex=getRunningEvent(datetime.unixtime());
		if(eventIndex!=-1){
			diff=getEndDifference(eventIndex, datetime.unixtime());
			isEvent=true;
			isEventRunning=true;
			display.setTextColor(RED);
		}
	}
	if(!isEvent){
		eventIndex=getNextEvent(datetime.unixtime());
		if(eventIndex!=-1){
			//show not running yet
			diff=getStartDifference(eventIndex, datetime.unixtime());
			isEvent=true;
			display.setTextColor(GREEN);
		}
	}

	//show message if no event or event does not end within 60 seconds
	if(datetime.unixtime() < messageStart+messageDuration) isMessage = true;
	if(datetime.unixtime() < videoStart+videoDuration)     isVideo   = true;

	display.waitUntilFlushed();
	display.clearBuffer();

	if(
		isEvent && (
			(((diff/60)%2)==0) || (diff<120)
		)
	){
		disableVideo();
		//show event
		showEvent(eventIndex, diff);

		if (isEventRunning)showProgressBar(eventIndex);
		//blink background 5 seconds before start
		if(
			(diff&1)&&(diff>=0)&&(diff<=10)
		)blink();

	/*
	} else if (isMessage){
		disableVideo();
		display.setBorder(0,XRES);
		display.setTextColor(YELLOW);
		display.scrollString(getMessage());
		if (isEventRunning)showProgressBar(eventIndex);
		diff=messageDuration;
	}else if (isVideo){
		enableVideo();
		if(isVideoEnabled()) display.copyBuffer((uint8_t*)message);
		
	*/		
	}else{
		disableVideo();
		//show time
		display.setTextColor(YELLOW);
		display.resetBorder();
		display.printDate(15, datetime.hour(), datetime.minute(), datetime.second());
		if (isEventRunning)showProgressBar(eventIndex);
	}

	 //if(!isVideoEnabled()){
	if(!isWifiConnected)display.rotateStar();
	display.flush();
	//}
	//display.softScreen();
}

void RadioEvent::showEvent(int8_t index, uint32_t diff){
	if (index<0)return;
	showTimeAndText(index, getDays(diff), getHours(diff), getMinutes(diff), getSeconds(diff));
}

void RadioEvent::showTimeAndText(int8_t index, int8_t days, int8_t hours, int8_t minutes, int8_t seconds){
	uint8_t xpos=0;
	display.resetBorder();
	xpos=display.printDuration(xpos, days, hours, minutes, seconds);
	xpos++;
	display.setBorder(xpos, XRES);
	if(xpos>32){
		//% (2*XRES)
		//int16_t scrollPos=XRES-((getMilliSeconds()/display.getScrollSpeed()) % (XRES + 30*5));
		//int8_t scrollPos=seconds-30;
		int8_t length=getEventTitleLength(index);
		int16_t scrollPos=XRES-( (getMilliSeconds()/display.getScrollSpeed()) % (XRES-xpos+length*5) );
		xpos=printEventTitleToDisplay(index, scrollPos);
	}else{
		xpos=printEventTitleToDisplay(index, xpos);
	}
}

void RadioEvent::printEvent(int8_t index, char *line){
	if (index<0)return;
	memset(line, 0x00, SOCKET_BUFFER_LENGTH);
	if(index<MAX_EVENTS-1){
		snprintf((char*)line, SOCKET_BUFFER_LENGTH,
			"%d %lu %lu ",
			index,
			radioEvent.getEventStart(index),
			radioEvent.getEventEnd(index)
		);
	}
	uint8_t xpos=strlen(line);
	if(xpos<0)xpos=0;
	if (index<2){
		xpos=radioEvent.printEventTitleToString(index, line+xpos);
	}		
	//snprintf((char*)line+xpos, SOCKET_BUFFER_LENGTH, "\n");
}

int8_t RadioEvent::getRunningEvent(uint32_t unixtime){
	for (uint8_t index=0;index<MAX_EVENTS-1;index++){
		if(eventExists(index) and getEventStart(index)<=unixtime and unixtime<=getEventEnd(index)){
			return index;
		}
	}
	return -1;
}

int8_t RadioEvent::getNextEvent(uint32_t unixtime){
	for (uint8_t index=0;index<MAX_EVENTS;index++){
		if(eventExists(index) and getEventStart(index)>=unixtime){
			return index;
		}
	}
	return -1;
}

void RadioEvent::setEvent(int8_t index, uint32_t start, uint32_t end, unsigned char *name){	
	if (index<0)return;
	if (index<MAX_EVENTS)	setEventStart(index,start);
	if (index<MAX_EVENTS-1)	setEventEnd(index,end);
	if(index<2)		setEventTitle(index,name);
}

void RadioEvent::setVideoRequest(uint8_t duration){
	videoStart=RTC.now().unixtime();
	videoDuration=duration;
}

void RadioEvent::setMessage(uint8_t duration, unsigned char *text){
	messageStart=RTC.now().unixtime();
	messageDuration=duration;
	clearMessage();
	uint8_t i=0;
	while((i<LINE_LENGTH-1) && (text[i]!=0) && (text[i]!=13) && (text[i]!=10)){
		message[i]=text[i];
		i++;
	}
	message[i]=0;
}

RadioEvent radioEvent;
