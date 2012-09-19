#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#define XRES 64
extern volatile uint8_t *drawBuffer;
extern volatile uint8_t switchBuffersFlag;
extern volatile uint32_t globaluS;
extern volatile uint32_t globalmS;
void setupDisplay(void);
void clearBuffer(volatile uint8_t *buffer);
signed char drawString(volatile uint8_t *buffer, signed char pos, const char *string);
void getCopperBars(uint8_t *color, uint16_t t);
void colorBar(volatile uint8_t *buffer, const uint8_t *color);

#endif // DISPLAY_H
