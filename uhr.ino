#include "TimerOne.h"
#include "character.h"
#include "SPI.h"


//#define DEBUG 


#include "display.h"



signed char t = XRES;

void displayTestImage(void) {
#ifdef DEBUG
  Serial.println("displayTestImage:");
#endif  
  drawString(displayBuffer[0], 0, "0123456789", orangeMask);
  drawString(displayBuffer[1], 0 ,"0123456789", greenMask);
  drawString(displayBuffer[2], 0, "0123456789", greenMask);
#ifdef DEBUG
  Serial.println();
  dumpBuffer(displayBuffer[0]);
  Serial.println();
  dumpBuffer(displayBuffer[0],1);
  Serial.println();
  dumpBuffer(displayBuffer[0],2);
#endif
}

void setup() {
#ifndef DEBUG
  setupDisplay();
  Timer1.initialize(200);
#else
  Timer1.initialize(10000);
  Serial.begin(19200);
  Serial.println("start");
#endif
  displayTestImage();
#ifndef DEBUG
  Timer1.attachInterrupt(displayCallback);
#endif
}

void loop() {
  clearBuffer(displayBuffer[0]);
  drawString(displayBuffer[0], t, "0123456789", orangeMask);
  clearBuffer(displayBuffer[1]);
  drawString(displayBuffer[1], t ,"0123456789", greenMask);
  clearBuffer(displayBuffer[2]);
  drawString(displayBuffer[2], t, "0123456789", greenMask);
  t--; if (t==-120) t=XRES;
  delay(300);

  
}
