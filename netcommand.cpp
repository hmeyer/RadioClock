//#include <string.h>
#include <stdlib.h>
//#include <stdio.h>
#include "WiShield/socketapp.h"
#include "WiShield/WiShield.h"
//#include "wiring.h"
//#include "rtc/RTClib.h"
#include "netcommand.h"
#include "display.h"
#include "display_timer.h"
#include "RadioEvent.h"
#include <util/atomic.h>

extern volatile bool DEBUG;

NetCommand Commander;
extern "C" {
	void handleCommand(const char *cmd, char* output) {
		return Commander.handleCommand(cmd, output);
	}
}

//parse next number from line
uint32_t NetCommand::parse_number(uint8_t &pos){
    uint32_t result=0;
    uint8_t  size=0;
    uint8_t  val=cmd[pos]-48;
    //number should be at last 15 characters long
    while (
      (val>=0) and (val<=9) and (size<11) and (pos<MESSAGE_LENGTH)
    ){
        //result=result*10+val;
        result=(result<<3)+(result<<1)+val;
        pos++;
        val=cmd[pos]-48;
	size++;
    }
    return result;
}

void NetCommand::printOk(){
	snprintf(output, SOCKET_OUTPUT_BUFFER_LENGTH, "ok\n");
}

void NetCommand::setEvent(){
	uint8_t pos=2;
        int8_t index =parse_number(pos);
	pos++;
	if (index>MAX_EVENTS-1)return;
	uint32_t start =parse_number(pos);
	pos++;
	uint32_t end   =parse_number(pos);
	pos++;
	radioEvent.setEvent(index, start, end, cmd+pos);
	printOk();
}

inline void NetCommand::setMessage(){
	radioEvent.messageLock=1;
	uint8_t pos=2;
        uint8_t duration =parse_number(pos);
	pos++;
        uint8_t scroll =parse_number(pos);
	pos++;
	radioEvent.setMessage(duration, scroll, cmd+pos);
	printOk();
	radioEvent.messageLock=0;
}


inline void NetCommand::setUnixTime(){
	radioEvent.clockLock=1;
	uint8_t pos=2;
        uint32_t newTime =parse_number(pos);
	RTC.adjust(DateTime(newTime));
	int32_t delta=newTime-radioEvent.unixTime;
	radioEvent.systemStart+=delta;
	radioEvent.requestTime+=delta;
	radioEvent.unixTime=newTime;
	printOk();
	radioEvent.clockLock=0;
}

inline void NetCommand::getUnixTime(){
	radioEvent.clockLock=1;
	snprintf(output, SOCKET_OUTPUT_BUFFER_LENGTH, 
		"%lu\n", 
		radioEvent.unixTime
	);
	radioEvent.clockLock=0;
}

inline void NetCommand::setScrollSpeed(){
	uint8_t pos=2;
        display.setScrollSpeed(parse_number(pos));
	printOk();
}


inline void NetCommand::setBrightness(){
	uint8_t pos=2;
	display.setBrightness(parse_number(pos));
	printOk();
}


inline void NetCommand::drawImage(uint8_t brightness){
	if(!radioEvent.isVideoEnabled()){
		snprintf(output, SOCKET_OUTPUT_BUFFER_LENGTH, "BRK\n");
		return;
	}
	radioEvent.videoLock=1;
	radioEvent.copyBuffer(cmd+1);
	radioEvent.videoLock=0;
	printOk();
}

inline void NetCommand::isVideoEnabled(){
	uint8_t flag=0;
	if(radioEvent.isVideoEnabled())flag=1;
	
	snprintf(output, SOCKET_OUTPUT_BUFFER_LENGTH, 
		"%u\n", 
		flag
	);
}

inline void NetCommand::enableVideo(){
	radioEvent.enableVideo();
	printOk();
}

inline void NetCommand::disableVideo(){
	radioEvent.disableVideo();
	printOk();
}

inline void NetCommand::setVideoRequest(){
	uint8_t pos=2;
        uint16_t duration =parse_number(pos);
	radioEvent.setVideoRequest(duration);
	printOk();
}


inline void NetCommand::getSystemStart(){
	snprintf(output, SOCKET_OUTPUT_BUFFER_LENGTH,
		"%lu %u %lu %lu\n",
		radioEvent.systemStart,
		radioEvent.reconnects,
		radioEvent.cycles,
		wifi.cycles
		//getMilliSeconds()
	);
}

void NetCommand::getSettings(){
	snprintf(output, SOCKET_OUTPUT_BUFFER_LENGTH,
		"%d %d %d %d %d %d\n",
		MAX_EVENTS, 
		TITLE_LENGTH, 
		MESSAGE_LENGTH,
		MAX_FRAMES,
		display.scrollSpeed,
		display.brightness
	);
}

void NetCommand::handleCommand(const char *cmd, char* output) {
	this->cmd=cmd;
	this->output=output;

	//ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		radioEvent.netLock=1;
		if      (*cmd == 'a') drawImage(0);
		else if (*cmd == 'b') drawImage(1);

		else if (*cmd == 'c') setScrollSpeed();

		else if (*cmd == 'e') getSettings();
		else if (*cmd == 'f') getSystemStart();

		else if (*cmd == 'h') isVideoEnabled();
		else if (*cmd == 'i') setVideoRequest();
		else if (*cmd == 'o') enableVideo();
		else if (*cmd == 'p') disableVideo();

		else if (*cmd == 'k') setBrightness();

		else if (*cmd == 'm') setMessage();

		else if (*cmd == 'u') setEvent();

		else if (*cmd == 'z') setUnixTime();
		else if (*cmd == 't') getUnixTime();
		else snprintf(output, SOCKET_OUTPUT_BUFFER_LENGTH, ERROR);
	//}
	output[SOCKET_OUTPUT_BUFFER_LENGTH-2]='\n';
	output[SOCKET_OUTPUT_BUFFER_LENGTH-1]=0;
	radioEvent.setRequestTime();
	radioEvent.netLock=0;
};

