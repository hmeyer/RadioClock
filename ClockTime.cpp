#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "display.h"
#include "ClockTime.h"

ClockTime clock;

#define MIN 60UL
#define HOUR (MIN*60UL)
#define DAY (HOUR*24UL)

uint8_t printinternal(char *dest, uint32_t duration);

ClockTime::ClockTime():offset(0),scale(0),oldticks(0),oldtime(0) {
}

void ClockTime::setTime(uint64_t now){
	uint64_t currentticks = getCurrent_ticks();

	oldticks = currentticks - oldticks;
	oldtime = now - oldtime;
	scale = double(oldtime) / double(oldticks);

	oldticks = currentticks;
	oldtime = now;

	offset = now - uint64_t(currentticks*scale);
}

uint64_t strtou64(char *s, char **e) {
	*e = s;
	uint64_t r = 0;
	while(((**e) <= '9') && ((**e) >= '0')) {
		r *= 10;
		r += (**e) - '0';
		(*e)++;
	}
	return r;
}

bool ClockTime::setTime(char *now, char *res){
	char *e;
	uint64_t n = strtou64(now, &e);
	sprintf(res, "set:");
	setTime(n);
	printinternal(res+4, n/1000);
	return true;
}

uint32_t ClockTime::getTime(){
	return ((int64_t)(getCurrent_ticks()*scale)+offset)/1000;
}

uint8_t ClockTime::print(char *dest) {
	printinternal(dest, this->getTime());
}

uint8_t printinternal(char *dest, uint32_t duration){
  *dest = 0;
  uint8_t t = 0;

  duration = duration % DAY;

  uint32_t r = duration / HOUR;
  duration -= r * HOUR;
  t += sprintf(dest + t,"%02u:", r);

  r = duration / MIN;
  duration -= r * MIN;
  t += sprintf(dest+t,"%02u:", r);

  t += sprintf(dest+t,"%02u",duration);
  return t;
}

