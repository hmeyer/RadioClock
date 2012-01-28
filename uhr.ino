#include "TimerOne.h"

volatile long ledcntr = 0;
volatile byte bits = 0;

#define COL_D A0
#define COL_CP A1
#define COL_OE A2
#define COL_ST A3

#define ROW_D 12
#define ROW_CP 13
#define ROW_OE 10
#define ROW_ST 11


void colshift(boolean v1, boolean v2) {
  if (v1)
    digitalWrite(COL_D, true);
  else digitalWrite(COL_D, false);
  digitalWrite(COL_CP, true);
  digitalWrite(COL_CP, false);
  if (v2)
    digitalWrite(COL_D, true);
  else digitalWrite(COL_D, false);
  digitalWrite(COL_CP, true);
  digitalWrite(COL_CP, false);
}

void rowshift(boolean v) {
  if (v)
    digitalWrite(ROW_D, 1);
  else digitalWrite(ROW_D, 0);
  digitalWrite(ROW_CP, 1);
  digitalWrite(ROW_CP, 0);
}


void callback() {
  if (ledcntr > 100) {
    ledcntr = 0;
    if ((bits & 7) == 0) {
      colshift(true, false);
      if ((bits & 63) == 0)
        rowshift(false);
      else rowshift(true);
    }
    else colshift(false, false);
    bits++;
  }
  ledcntr++;
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(COL_D, OUTPUT);
  pinMode(COL_CP, OUTPUT);
  pinMode(COL_OE, OUTPUT);
  pinMode(COL_ST, OUTPUT);
  digitalWrite(COL_OE, true);
  digitalWrite(COL_ST, true);
  pinMode(ROW_D, OUTPUT);
  pinMode(ROW_CP, OUTPUT);
  pinMode(ROW_OE, OUTPUT);
  pinMode(ROW_ST, OUTPUT);
  digitalWrite(ROW_OE, true);
  digitalWrite(ROW_ST, true);

  for(int i = 0; i < 8; i++) {
    colshift(false, false);
    rowshift(true);
  }
  Timer1.initialize(100);
  Timer1.attachInterrupt(callback);
}

void loop() {
}
 
