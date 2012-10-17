#include <stdint.h>
#include "display.h"
#include "timing.h"
#include "scroller.h"
#include "wiring.h"
#include "switch.h"
#include "WiShield/WiShield.h"
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

extern "C" {
#include "serial/uart.h"
}


uint16_t t = 0;
uint8_t colorbars[8];
int8_t bcnt = 0;

int main() {
    uint8_t c;
    char buffer[7];
    int  num=134;
    uart_init( UART_BAUD_SELECT(BAUD,F_CPU) ); 
    sei();
    uart_puts("String stored in SRAM\n");
    uart_puts_P("String stored in FLASH\n");
    /* 
     * Use standard avr-libc functions to convert numbers into string
     * before transmitting via UART
     */     
    itoa( num, buffer, 10);   // convert interger into string (decimal format)         
    uart_puts(buffer);        // and transmit string to UART
    /*
     * Transmit single character to UART
     */
    uart_putc('\r');

    for(;;)
    {
        /*
         * Get received character from ringbuffer
         * uart_getc() returns in the lower byte the received character and 
         * in the higher byte (bitmask) the last receive error
         * UART_NO_DATA is returned when no data is available.
         *
         */
        if (uart_getc(&c) == 0) {
	     /*
             * new data available from UART
             * send received character back
             */
            uart_puts_P("got char: ");
            uart_putc( (unsigned char)c );
            uart_puts( "\n\r" );
        }
        else
        {
            /* 
             * no data available from UART 
             */
        }
    }

  
  setupDisplay();
  setupSwitch();
  uint16_t stop=0;

  char mystring[] = "08";
  WiFi.initPre();
  while(WiFi.initLoop()) {
          uint8_t red[] = {3,3,3,3,3,3,3,3};
	  while(switchBuffersFlag);
	  clearBuffer(drawBuffer);
	  scrollString(drawBuffer, "setup", globalmS/50);
	  colorBar(drawBuffer, red);
	  switchBuffersFlag = 1;
  }
  WiFi.initPost();
  while(1) {
  	  updateSwitch();
	  while(switchBuffersFlag);
	  clearBuffer(drawBuffer);
	  if (!stop) *mystring = ((globaluS/1000000)%10)+'0';
	  else stop--;

	  char bChar=0;

	  if (switchPressed(SW_F_UP)) bChar = 'U';
	  if (switchPressed(SW_F_DOWN)) bChar = 'D';
	  if (switchPressed(SW_F_LEFT)) bChar = 'L';
	  if (switchPressed(SW_F_RIGHT)) bChar = 'R';
	  if (switchPressed(SW_F_PUSH)) bChar = 'P';
	  if (bChar =='L') { *mystring=(--bcnt)+'0'; stop = 200; }
	  else if (bChar =='R') { *mystring=(++bcnt)+'0'; stop = 200; }
	  else if (bChar =='P') { *mystring=(bcnt)+'0'; stop = 200; }
	  else if (bChar) { *mystring = bChar; stop = 200; }


	  drawString(drawBuffer, 0, mystring);
	  getCopperBars( colorbars, t/4 );
	  colorBar(drawBuffer, colorbars);
	  switchBuffersFlag = 1;
	  t++;
  }

}
