

#ifndef CLOCKTIME_H_
#define CLOCKTIME_H_

#include <inttypes.h>

class ClockTime{
	uint64_t offset;

	public:
	ClockTime();

	void setTime(uint64_t now);

	uint64_t getTime();

	void print(char *dest, uint32_t duration);
};

extern ClockTime clock;

#endif