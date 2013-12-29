#ifndef WIRING_H
#define WIRING_H

#include <stdint.h>

#define INPUT 0x0
#define OUTPUT 0x1


void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, bool val);
bool digitalRead(uint8_t pin);

#endif // WIRING_H
