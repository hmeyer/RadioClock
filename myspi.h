#ifndef MYSPI_H
#define MYSPI_H

void SPIsetup(void);
void SPItransfer(uint8_t data);
void SPItransferBufferReverse(volatile uint8_t *data, uint8_t count);
bool SPIfinished() { extern volatile uint8_t SPIBufferCount; return SPIBufferCount==0; }

#endif
