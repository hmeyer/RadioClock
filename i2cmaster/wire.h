#ifndef _I2C_H
#define _I2C_H

#include <avr/io.h>
#include <inttypes.h>

extern "C" {
#include "i2cmaster.h"
}

class WIRE {
public:

inline void begin(void) {
	i2c_init();
}
inline void beginTransmission(uint8_t address) {
	i2c_start_wait( (address << 1) | I2C_WRITE );
}
inline uint8_t endTransmission(bool send_stop=true) {
	if (send_stop) i2c_stop();
	return 0;
}
inline void write(uint8_t data) {
	i2c_write( data );
}
uint8_t requestFrom(uint8_t address, uint8_t num, bool send_stop=true) {
	i2c_start_wait( (address << 1) | I2C_READ );
	return 0;
}
inline uint8_t read() {
	return i2c_readAck();
}
};

extern WIRE Wire;


#endif 
