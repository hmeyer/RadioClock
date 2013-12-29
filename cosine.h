#ifndef COSINE_H
#define COSINE_H

#include <avr/io.h>
#include <avr/pgmspace.h>

extern const uint8_t _cosine[] PROGMEM;
inline uint8_t cosine(uint8_t x) { return pgm_read_byte(&(_cosine[x])); }
#define 	M_PI   3.14159265358979323846 /* pi */
inline float cos(float x) { return ((cosine(128*x/M_PI)-128)/128.0); }
inline float sin(float x) { return cos(x-M_PI/2.0); }

#endif //COSINE_H

