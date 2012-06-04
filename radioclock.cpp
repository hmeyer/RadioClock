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
	  drawString(drawBuffer, 8-((t/3)%70), "0123456789");
	  getCopperBars( colorbars, t/4 );
	  colorBar(drawBuffer, colorbars);
	  switchBuffersFlag = 1;
	  t++;
  }
}
