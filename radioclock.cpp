#include <stdint.h>
#include "display.h"
#include "wiring.h"


uint16_t t = XRES;
uint8_t colorbars[8];

int main() {
  setupDisplay();

  while(1) {
	  while(switchBuffersFlag);
	  clearBuffer(drawBuffer);
	  char mystring[] = "08";
	  *mystring = ((globaluS/1000000)%10)+'0';
	  drawString(drawBuffer, 0, mystring);
	  getCopperBars( colorbars, t/4 );
	  colorBar(drawBuffer, colorbars);
	  switchBuffersFlag = 1;
	  t++;
  }
}
