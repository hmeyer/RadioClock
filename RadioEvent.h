#ifndef RADIO_EVENT_H
#define RADIO_EVENT_H

#include <stdint.h>
#include <string.h>
#include "display.h"

#define SEC 1
#define MIN 60*SEC
#define HOUR 60*MIN
#define DAY 24*HOUR
#define MONTH 30*DAY
#define YEAR 365*DAY

#define MAX_EVENTS 10
#define MAX_EVENT_TITLES 2
#define TITLE_LENGTH 60
#define MESSAGE_LENGTH 130
#define BLINK_TIME 10


typedef struct {
	uint32_t start;
} Event;

#define ERROR "ERR\n"

class RadioEvent {
	public:
	RadioEvent();
	//list of shows
	volatile Event events [MAX_EVENTS];
	char eventTitles[MAX_EVENT_TITLES][TITLE_LENGTH];
	
	//text message
	uint32_t messageStart;
	uint8_t  messageDuration;
	bool     messageScroll;
	char     message[MESSAGE_LENGTH];

	//video
	uint32_t videoStart;
	uint16_t videoDuration;
	bool     videoAllowed;

	uint32_t unixTime;
	uint32_t requestTime;
	uint32_t systemStart;
	uint16_t reconnects;
	uint32_t cycles;

	uint8_t netLock;
	uint8_t clockLock;
	uint8_t messageLock;
	uint8_t videoLock;

	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	
	void init(void);

	//event functions
	void copyBuffer(const char* data);

	void updateTime();
	void showCurrentEvent(const bool isWifiConnected);
	void showEvent(const int8_t index, const uint32_t diff);

	//index is integer, undefined=-1
	int8_t getNextEvent();
	int8_t getRunningEvent();

	void setEvent(const int8_t index, const uint32_t start, const uint32_t end, const char *name);
	void setEventTitle(int8_t index, const char *name);

	inline const char* getEventTitle(const int8_t index){
		if((index<0)||(index>=MAX_EVENT_TITLES))return ERROR;
		return eventTitles[index];
	}

	inline int16_t getEventTitleLength(const int8_t index){
		if((index<0)||(index>=MAX_EVENT_TITLES))return 6;
		return strlen(getEventTitle(index));
	}

	inline bool eventExists(const int8_t index){
		return ((index>=0) and (index<MAX_EVENTS) and (events[index].start>1380000000UL));
	}

	inline uint32_t getEventStart(const int8_t index){
		if(eventExists(index))return events[index].start;
		return 0;
	}
	inline uint32_t getEventEnd(const int8_t index){
		return getEventStart(index+1);
	}

	inline void setEventStart(const int8_t index, const uint32_t unixtime){
		if (index<MAX_EVENTS-1) events[index].start=unixtime;
	}
	inline void setEventEnd(const int8_t index, const uint32_t unixtime){
		if (index+1<MAX_EVENTS-1) events[index+1].start=unixtime;
		if (index+2<MAX_EVENTS-1) events[index+2].start=0;
	}

	inline void setRequestTime(){
		requestTime=unixTime;
	}

	inline void setSystemStart(){
		systemStart=unixTime;
	}

	//message functions
	inline void clearMessage(){
		memset(message, 0x00, MESSAGE_LENGTH);
	}
	
	void setMessage(const uint8_t duration, const bool scroll, const char *text);

	//video functions
	inline void enableVideo(){
		videoAllowed=true;
	}
	inline void disableVideo(){
		videoAllowed=false;
	}
	inline bool isVideoEnabled(){
		return videoAllowed;
	}
	void setVideoRequest(const uint16_t duration);

	void showProgressBar(const uint8_t eventIndex);
	void blink();

	inline uint32_t getDifference(const uint32_t time1, const uint32_t time2){
		if (time2>time1) return time2-time1;
		if (time1>time2) return time1-time2;
		return 0;
	}

	inline uint8_t getDays(const uint32_t duration){
		return (uint8_t)(duration/86400);
	}

	inline uint8_t getHours(const uint32_t duration){
		return (uint8_t)((duration/3600)%24);
	}

	inline uint8_t getMinutes(const uint32_t duration){
		return (uint8_t)((duration/60)%60);
	}

	inline uint8_t getSeconds(const uint32_t duration){
		return (uint8_t)(duration%60);
	}

};
extern RadioEvent radioEvent;
#endif // RADIO_EVENT_H


