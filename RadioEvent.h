#ifndef RADIO_EVENT_H
#define RADIO_EVENT_H

#include <stdint.h>
#include <string.h>

#define SEC 1
#define MIN 60*SEC
#define HOUR 60*MIN
#define DAY 24*HOUR
#define MONTH 30*DAY
#define YEAR 365*DAY

#define MAX_EVENTS 3
#define NAME_LENGTH 10
#define LINE_LENGTH 130


typedef struct {
	uint32_t start;
} Event;

class RadioEvent {
	public:
	//list of shows
	volatile Event events [MAX_EVENTS];
	unsigned char eventTitles[2][NAME_LENGTH];
	
	//text message
	uint32_t messageStart;
	uint8_t  messageDuration;
	char     message[LINE_LENGTH];

	//video
	uint32_t videoStart;
	uint8_t  videoDuration;
	bool     videoAllowed;

	
	void init(void);

	//event functions

	void showCurrentEvent(bool isWifiConnected);
	void showEvent(int8_t index, uint32_t diff);
	void showTimeAndText(int8_t index, char *string);//pre-rendered date,time and title
	void showTimeAndText(int8_t index, int8_t days, int8_t hours, int8_t minutes, int8_t seconds);

	//index is integer, undefined=-1
	int8_t getNextEvent(uint32_t unixtime);
	int8_t getRunningEvent(uint32_t unixtime);

	void setEvent(int8_t index, uint32_t start, uint32_t end, unsigned char *name);

	void printEvent(int8_t index, char *dest);
	void setEventTitle(int8_t index, unsigned char *name);
	int16_t printEventTitle(int8_t index, bool toDisplay, int16_t xpos, bool toString, char *dest);

	inline int16_t getEventTitleLength(int8_t index){
		return printEventTitle(index, false, 0, false, 0);
	}

	inline int16_t printEventTitleToDisplay(int8_t index, int16_t xpos){
		return printEventTitle(index, true, xpos, false, 0);
	}
	
	inline int16_t printEventTitleToString(int8_t index, char *dest){
		return printEventTitle(index, false, 0, true, dest);
	}

	inline bool eventExists(int8_t index){
		return ((index>=0) && (index<MAX_EVENTS) &&(events[index].start>0));
	}

	inline uint32_t getEventStart(int8_t index){
		return events[index].start;
	}
	inline uint32_t getEventEnd(int8_t index){
		if(index<MAX_EVENTS-1)return events[index+1].start;
		else return 0;
	}

	inline void setEventStart(int8_t index, uint32_t unixtime){
		events[index].start=unixtime;
	}
	inline void setEventEnd(int8_t index, uint32_t unixtime){
		events[index+1].start=unixtime;
	}

	//message functions
	inline void clearMessage(){
		memset((char*)message, 0x00, LINE_LENGTH);
	}

	void setMessage(uint8_t duration, unsigned char *text);

	inline char *getMessage(){
		return (char*)message;
	}
	inline uint32_t getMessageStart(){
		return messageStart;
	}

	inline uint8_t getMessageDuration(){
		return messageDuration;
	}

	//video functions
	inline void enableVideo(){
		if(!videoAllowed)videoAllowed=true;
	}
	inline void disableVideo(){
		if(videoAllowed)videoAllowed=false;
	}
	inline bool isVideoEnabled(){
		return videoAllowed;
	}
	void setVideoRequest(uint8_t duration);

	inline uint32_t getVideoStart(){
		return videoStart;
	}
	inline uint8_t getVideoDuration(){
		return videoDuration;
	}

	void showProgressBar(uint8_t eventIndex);
	void blink();

	//duration functions	
	inline uint32_t getStartDifference(int8_t index, uint32_t unixtime){
		return getDifference(getEventStart(index), unixtime);
	}
	inline uint32_t getEndDifference(int8_t index, uint32_t unixtime){
		return getDifference(getEventEnd(index), unixtime);
	}

	inline uint32_t getDifference(uint32_t time1, uint32_t time2){
		if(time1>=time2) return time1-time2;
		return time2-time1;
	}

	inline uint8_t getDays(uint32_t duration){
		return duration/86400;
	}

	inline uint8_t getHours(uint32_t duration){
		return (duration/3600)%24;
	}

	inline uint8_t getMinutes(uint32_t duration){
		return (duration/60)%60;
	}

	inline uint8_t getSeconds(uint32_t duration){
		return duration%60;
	}

};
extern RadioEvent radioEvent;
#endif // RADIO_EVENT_H


