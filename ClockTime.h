

#ifndef CLOCKTIME_H_
#define CLOCKTIME_H_

#include <inttypes.h>

class ClockTime{
	int64_t offset;
	double scale;
	uint64_t oldticks;
	uint64_t oldtime;

	public:
	ClockTime();

	void setTime(uint64_t now);
	bool setTime(char *now, char *res);

	uint32_t getTime();

	uint8_t print(char *dest);
};

extern ClockTime clock;

#endif
