// Implement SPI via USART
// Clock: PD4 - Arduino D4
// Output:  PD1 - Arduino D1

#include <stdint.h>
#include <avr/interrupt.h>
#include "wiring.h"
#include "bits.h"

#ifndef SERIAL

uint8_t SPIBufferCount;
volatile uint8_t *SPIBuffer;

void SPIsetup(void) {
  UBRR0 = 0;
  pinMode(4, OUTPUT); // set clock as output
  sbi(UCSR0C, UMSEL01); 
  sbi(UCSR0C, UMSEL00); 
  sbi(UCSR0C, UDORD0); // LSBFirst
  cbi(UCSR0C, UCPHA0); // Data Mode
  cbi(UCSR0C, UCPOL0); // Data Mode
  sbi(UCSR0B, TXEN0);  // Enable Transmit
  cbi(UCSR0B, UDRIE0);  // Disable Data Empty Interupt
  UBRR0 = 0; // max Speed
}

void SPItransfer(uint8_t data) {
  while( !(UCSR0A & _BV(UDRE0)) );
  UDR0 = data;
}

void SPItransferBufferReverse(volatile uint8_t *data, uint8_t count) {
  SPIBuffer = data;
  SPIBufferCount = count;
  SPIBufferCount--;
//  while( !(UCSR0A & _BV(UDRE0)) );
  UDR0 = data[SPIBufferCount];
  sbi(UCSR0B, UDRIE0); // Enable Data Empty Interupt
}

ISR(USART_UDRE_vect) {
  --SPIBufferCount;
  UDR0 = SPIBuffer[SPIBufferCount];
  if (!SPIBufferCount) cbi(UCSR0B, UDRIE0); // Disable Data Empty Interupt
}

#else
void SPIsetup(void) {}
void SPItransfer(uint8_t data) {}
void SPItransferBufferReverse(volatile uint8_t *data, uint8_t count) {}
#endif
