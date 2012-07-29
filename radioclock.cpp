#include <stdint.h>
#include "display.h"
#include "wiring.h"
#include "switch.h"


uint16_t t = XRES;
uint8_t colorbars[8];

int main() {
  setupDisplay();
  setupSwitch();

  while(1) {
  	  updateSwitch();
	  while(switchBuffersFlag);
	  clearBuffer(drawBuffer);
	  char mystring[] = "08";
	  *mystring = ((globaluS/1000000)%10)+'0';
	  if (switchPressed(SW_F_UP)) *mystring = 'U';
	  if (switchPressed(SW_F_DOWN)) *mystring = 'D';
	  if (switchPressed(SW_F_LEFT)) *mystring = 'L';
	  if (switchPressed(SW_F_RIGHT)) *mystring = 'R';
	  if (switchPressed(SW_F_PUSH)) *mystring = 'P';
	  
	  drawString(drawBuffer, 0, mystring);
	  getCopperBars( colorbars, t/4 );
	  colorBar(drawBuffer, colorbars);
	  switchBuffersFlag = 1;
	  t++;
  }
}
