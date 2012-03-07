#include "character.h"


//#define DEBUG 
#include "display.h"



uint16_t t = XRES;
uint8_t colorbars[8];

void setup() {
#ifndef DEBUG
  setupDisplay();
#else
  Serial.begin(19200);
  Serial.println("start");
#endif
}

void loop() {
  while(switchBuffersFlag) delay(1);
  clearBuffer(drawBuffer);
  drawString(drawBuffer, 8-(t%70), "0123456789");
  getCopperBars( colorbars, t );
  colorBar(drawBuffer, colorbars);
  switchBuffersFlag = 1;
#ifdef DEBUG
  dumpBuffer(displayBuffer);
#endif
  t++;
  delay(100);

  
}
