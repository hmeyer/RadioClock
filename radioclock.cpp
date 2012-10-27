#include <stdint.h>
#include "display.h"
#include "scroller.h"
#include "wiring.h"
#include "switch.h"
#include "WiShield/WiShield.h"


uint16_t t = 0;
uint8_t colorbars[8];
int8_t bcnt = 0;

int main() {
  setupDisplay();
  setupSwitch();
  uint16_t stop=0;

  char mystring[] = "08";
  WiFi.initPre();
  while(WiFi.initLoop()) {
          uint8_t red[] = {3,3,3,3,3,3,3,3};
	  while(switchBuffersFlag);
	  clearBuffer(drawBuffer);
	  scrollString(drawBuffer, "setup", getCurrent_ms()/50);
	  colorBar(drawBuffer, red);
	  switchBuffersFlag = 1;
  }
  WiFi.initPost();
  while(1) {
  	  updateSwitch();
	  while(switchBuffersFlag);
	  clearBuffer(drawBuffer);
	  if (!stop) *mystring = ((getCurrent_ms()/1000)%10)+'0';
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
