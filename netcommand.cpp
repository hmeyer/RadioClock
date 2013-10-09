//#include <string.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include "WiShield/socketapp.h"
//#include "wiring.h"
//#include "rtc/RTClib.h"
#include "netcommand.h"
#include "display.h"
#include "RadioEvent.h"

#define SOCKET_BUFFER_LENGTH 130

extern volatile bool DEBUG;
NetCommand Commander;

extern "C" {
	void handleCommand(char *cmd) {
		return Commander.handleCommand(cmd);
	}
}

//parse next number from line
uint8_t pos=0;
uint32_t NetCommand::parse_number(char *line){
    uint32_t result=0;
    uint8_t  size=0;
    uint8_t  val=line[pos]-48;
    //number should be at last 15 characters long
    while (
      (val>=0) && (val<=9) && (size<15) && (pos<LINE_LENGTH)
    ){
        result*=10;
        result+=val;
        pos++;
	size++;
        val=line[pos]-48;
    }
    return result;
}

void inline NetCommand::clear_line(char *line){
	memset(line, 0x00, SOCKET_BUFFER_LENGTH);
}


void NetCommand::setEvent(char *line){
	pos=1;
        int8_t index =parse_number(line);
	pos++;
	if (index>MAX_EVENTS-1)return;
	uint32_t start =parse_number(line);
	pos++;
	uint32_t end   =parse_number(line);
	pos++;
	radioEvent.setEvent(index, start, end, (unsigned char*)line+pos);
	clear_line(line);
	snprintf((char*)line, SOCKET_BUFFER_LENGTH, 
		"%d %lu %lu\n",
		index, 
		radioEvent.getEventStart(index), 
		radioEvent.getEventEnd(index)
	);
}

void NetCommand::getEvent(char *line){
	pos=1;
        uint8_t index =parse_number(line);
	pos++;
	if (index>=MAX_EVENTS)return;
	clear_line(line);
	radioEvent.printEvent(index, (char*)line);
	uint16_t size=strlen((char*)line);
	if(size>100)size=100;
	line[size]='\n';
	line[size+1]=0;
}

void NetCommand::getSettings(char *line){
	clear_line(line);
	snprintf((char*)line, SOCKET_BUFFER_LENGTH,
		"%d %d %d\n",
		MAX_EVENTS, 
		NAME_LENGTH, 
		LINE_LENGTH
	);
}

inline void NetCommand::setMessage(char *line){
	pos=2;
        uint8_t duration =parse_number(line);
	pos++;
	radioEvent.setMessage(duration, (unsigned char*)line+pos);
}

inline void NetCommand::getMessage(char *line){
	clear_line(line);
	snprintf((char*)line, SOCKET_BUFFER_LENGTH,
		"'%lu' '%u' '%s'\n",
		radioEvent.getMessageStart(), 
		radioEvent.getMessageDuration(), 
		radioEvent.getMessage()
	);
}

inline void NetCommand::setUnixTime(char *line){
	pos=1;
        uint32_t time =parse_number(line);
	pos++;
        //uint32_t millis =parse_number(line);
	
	RTC.adjust(DateTime(time));
	clear_line(line);
	snprintf((char*)line, SOCKET_BUFFER_LENGTH, "ok\n");
}

inline void NetCommand::getUnixTime(char *line){
	pos=0;
  	DateTime d = RTC.now();

	clear_line(line);
	snprintf((char*)line, SOCKET_BUFFER_LENGTH, 
		"%lu\n", 
		d.unixtime()
	);
}

inline void NetCommand::setScrollSpeed(char *line){
	pos=1;
        display.setScrollSpeed(parse_number(line));
	clear_line(line);
	snprintf((char*)line, SOCKET_BUFFER_LENGTH, "ok\n");
}

inline void NetCommand::getScrollSpeed(char *line){
	clear_line(line);
	snprintf((char*)line, SOCKET_BUFFER_LENGTH, "%d\n", display.getScrollSpeed());
}

inline void NetCommand::setBrightness(char *line){
	pos=1;
	display.setBrightness(parse_number(line));
	clear_line(line);
	snprintf((char*)line, SOCKET_BUFFER_LENGTH, "ok\n");
}

inline void NetCommand::getBrightness(char *line){
	clear_line(line);
	snprintf((char*)line, SOCKET_BUFFER_LENGTH,
		"'%d'\n",
		display.getBrightness()
	);
}


inline void NetCommand::drawImage(char *line, uint8_t brightness){
	if(!radioEvent.isVideoEnabled())return;
	//volatile uint8_t *drawBuffer2 = display.getDisplayBuffer();
	//if(drawBuffer!=drawBuffer2) drawBuffer  = display.getDisplayBuffer();
	//volatile uint8_t *drawBuffer = display.getDrawBuffer();
	
	//for(uint8_t i=0;i < XRES * 2;i+=8) drawBuffer[i]=0;
	//for(uint8_t i=0;i < XRES * 2;i+=8) drawBuffer[i+4*XRES]=0;
	//for(uint8_t i=0;i < 128;i++)radioEvent.string[i]=line[i];

	//volatile uint8_t *drawBuffer  = display.getDisplayBuffer();

	//works
	memcpy(radioEvent.message, line, 128);
	//for(uint8_t i=0;i < 128;i++) radioEvent.message[i]=line[i];

	/*
	//works not
	display.waitUntilFlushed();
	display.clearBuffer();
	display.copyBuffer((uint8_t*)line);
	display.flush();
	*/

	//if(brightness==0)memcpy((void*)drawBuffer,line,128);
	//if(brightness==1)memcpy((void*)(drawBuffer+4*XRES),line,128);

	/*
		if (radioEvent.isVideoEnabled()){
			display.getDrawBuffer()[i]=line[i]&255;
			//drawBuffer[i]=line[i];
			//drawBuffer[i]=(char)line[i];
			line[i]=0;
		}
	}
	*/

	//display.clearBuffer();
	//memset(radioEvent.string, 0x00, 128);
	//if(brightness==0)
	//memcpy(radioEvent.string, line, 128);
	//if(brightness==0)memcpy((void*)drawBuffer+4*XRES,line,64);
	//if(brightness==0)memcpy((void*)drawBuffer,line,128);
	//if(brightness==1)memcpy((void*)(drawBuffer+4*XRES),line,128);
	//memset(line, 0x00, SOCKET_BUFFER_LENGTH-1);
	//snprintf((char*)line, SOCKET_BUFFER_LENGTH-1, "\n");
	line[0]='\n';
	line[1]=0;
}

void NetCommand::setVideoPermission(char *line){
	pos=1;
	uint8_t video=parse_number(line);
	if(video==0)radioEvent.disableVideo();
	else radioEvent.enableVideo();

	clear_line(line);
	snprintf((char*)line, SOCKET_BUFFER_LENGTH, 
		"%d\n", 
		video
	);
}

inline void NetCommand::isVideoEnabled(char *line){
	uint8_t flag=0;
	if(radioEvent.isVideoEnabled())flag=1;
	clear_line(line);
	snprintf((char*)line, SOCKET_BUFFER_LENGTH, 
		"%d\n", 
		flag
	);
}

inline void NetCommand::setVideoRequest(char *line){
	pos=1;
        uint8_t duration =parse_number(line);
	pos++;
	radioEvent.setVideoRequest(duration);
}

inline void NetCommand::getVideo(char *line){
	clear_line(line);
	snprintf((char*)line, SOCKET_BUFFER_LENGTH,
		"'%lu' '%u'\n",
		radioEvent.getVideoStart(), 
		radioEvent.getVideoDuration()
	);
}

void NetCommand::handleCommand(char *cmd) {
	if      (*cmd == 'a') drawImage(cmd+1, 0);
	else if (*cmd == 'b') drawImage(cmd+1, 1);

	else if (*cmd == 'i') setVideoRequest(cmd);
	else if (*cmd == 'j') getVideo(cmd);
	//else if (*cmd == 'g') setVideoPermission(cmd);
	else if (*cmd == 'h') isVideoEnabled(cmd);

	else if (*cmd == 'k') setBrightness(cmd);
	else if (*cmd == 'l') getBrightness(cmd);

	else if (*cmd == 'm') setMessage(cmd);
	else if (*cmd == 'n') getMessage(cmd);

	else if (*cmd == 'u') setEvent(cmd);
	else if (*cmd == 's') getEvent(cmd);

	else if (*cmd == 'c') setScrollSpeed(cmd);
	else if (*cmd == 'd') getScrollSpeed(cmd);

	else if (*cmd == 'z') setUnixTime(cmd);
	else if (*cmd == 't') getUnixTime(cmd);

	else if (*cmd == 'e') getSettings(cmd);
};

