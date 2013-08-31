#ifndef CHARARCTER_H
#define CHARARCTER_H

#include <avr/io.h>
#include <avr/pgmspace.h>

extern const unsigned char characters[] PROGMEM;
inline const uint8_t charset(uint8_t c, uint8_t line) { return pgm_read_byte(&(characters[(c<<3)+line])); }

#define mono2HighByte(c) ((uint8_t)\
  (c&0x80) | ((c>>1)&0x20) | ((c>>2)&0x08) | ((c>>3)&0x02)\
)

#define mono2LowByte(c) ((uint8_t)\
  (c&0x01) | ((c<<1)&0x04) | ((c<<2)&0x10) | ((c<<3)&0x40)\
)

#define mono2HighColorByte(c) ((uint8_t)mono2HighByte(c)|(mono2HighByte(c)>>1))
#define mono2LowColorByte(c) ((uint8_t)mono2LowByte(c)|(mono2LowByte(c)<<1))

#endif //CHARACTER_H

