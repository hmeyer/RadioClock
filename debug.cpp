
#include "display.h"

extern "C" {
void debug(char *m) {
	  DISP.waitUntilFlushed();
	  DISP.clearBuffer();
	  DISP.drawString(0, m);
  	  uint8_t c[] = {3,12,3,12,3,12,3,12};
	  DISP.colorBar(c);
	  DISP.flush();
}
void debugL(unsigned l) {
	  DISP.waitUntilFlushed();
	  DISP.clearBuffer();
	  DISP.plot(l%50, l/50, 1,1);
  	  uint8_t c[] = {3,12,3,12,3,12,3,12};
	  DISP.colorBar(c);
	  DISP.flush();
}
}
