
#include "display.h"

extern "C" {
void debug(char *m) {
	  display.waitUntilFlushed();
	  display.clearBuffer();
	  display.drawString(0, m);
  	  //uint8_t c[] = {3,12,3,12,3,12,3,12};
	  //display.colorBar(c);
	  display.flush();
}
void debugL(unsigned l) {
	  display.waitUntilFlushed();
	  display.clearBuffer();
	  display.plot(l%50, l/50, 1,1);
  	  //uint8_t c[] = {3,12,3,12,3,12,3,12};
	  //display.colorBar(c);
	  display.flush();
}
}
