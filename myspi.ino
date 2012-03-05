// Implement SPI via USART
// Clock: PD4 - Arduino D4
// Output:  PD1 - Arduino D1

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif 

void SPIsetup(void) {
  UBRR0 = 0;
  pinMode(4, OUTPUT); // set clock as output
  sbi(UCSR0C, UMSEL01); 
  sbi(UCSR0C, UMSEL00); 
  sbi(UCSR0C, UDORD0); // LSBFirst
  cbi(UCSR0C, UCPHA0); // Data Mode
  cbi(UCSR0C, UCPOL0); // Data Mode
  sbi(UCSR0B, TXEN0); 
  UBRR0 = 0; // max Speed
}

void SPItransfer(uint8_t data) {
  while( !(UCSR0A & _BV(UDRE0)) );
  UDR0 = data;
}

void SPItransferBufferReverse(volatile uint8_t *data, uint8_t count) {
  do {
    count--;
    SPItransfer(data[count]);
  } while(count);
}
