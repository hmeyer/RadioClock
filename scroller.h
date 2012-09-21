#ifndef SCROLLER_H
#define SCROLLER_H

#include <stdint.h>

void scrollString(volatile uint8_t *buffer, const char *string, int16_t counter);

#endif 
