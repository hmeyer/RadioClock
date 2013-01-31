#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "display.h"
#include "ClockTime.h"

ClockTime clock;

#define SEC 1;
#define MIN 60;
#define HOUR 3600;
#define DAY 86400;

ClockTime::ClockTime():offset(0){
}

void ClockTime::setTime(uint64_t now){
	offset=now-getCurrent_ms();
}

uint64_t ClockTime::getTime(){
	return getCurrent_ms()+offset;
}

void ClockTime::print(char *dest, uint32_t duration){
  char val[10]; 
  strcpy(dest,"");

  uint32_t days= duration / DAY;
  if (days>0){
    sprintf(val,"%ud ",days);
    strcat(dest,val);
    duration-=days*DAY;
  }

  uint32_t hours= duration / HOUR;
    sprintf(val,"%02u:",hours);
    strcat(dest,val);
    duration-=hours*HOUR;

  uint32_t minutes= duration / MIN;
//  if (days==0){
    sprintf(val,"%02u:",minutes);
    strcat(dest,val);
    duration-=minutes*MIN;
//  }

//  if (hours<1){
    sprintf(val,"%02u",duration);
    strcat(dest,val);
//  }

}
