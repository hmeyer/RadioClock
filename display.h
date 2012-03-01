#ifndef DISPLAY_H
#define DISPLAY_H

#define XRES 40
#define redMask 0b01010101
#define greenMask (redMask<<1)
#define orangeMask (redMask|greenMask)



volatile long ledcntr = 0;
volatile byte bits = 0;

#define COL_D 11
#define LINE_CLK 13
#define COL_OE 9
#define COL_ST 8

#define ROW_OE 4
#define ROW_ST 5
#define ROW_D 6
#define ROW_CP 7

#ifdef DEBUG
void dumpBuffer(volatile byte *buffer, byte rg=0) { // 0=both, 1=red, 2=green
  for(int y=0; y < 8; y++) {
    byte b;
    if (!rg)
      for(int x=0; x < XRES*2; x++) {
        if ((x&7) == 0) {
          b = buffer[ y*XRES/4 + x/8 ];
        }
        Serial.write( (b&128)?'@':'.' );
        b <<= 1;
      }
    else 
      for(int x=0; x < XRES; x++) {
        if ((x&3) == 0) {
          b = buffer[ y*XRES/4 + x/4 ];
          if (rg==1) b<<=1;
        }
        Serial.write( (b&128)?'@':'.' );
        b <<= 2;
      }
    Serial.println("");
  }
}
#endif

volatile byte displayBuffer[3][XRES * 2 ], drawBuffer[3][XRES * 2 ];

volatile byte line = 255;
volatile byte lineiters = 0;


inline void clearBuffer(volatile byte *buffer) {
  for(byte i=0;i < XRES * 2;i++)
    buffer[i]=0;
}

inline void drawChar(volatile byte *buffer, signed char &pos, byte charIdx, byte channelMask) {
  const signed char width = 5;
  if ((pos > -width) && (pos < XRES)) {
    signed char bufferIndex = (pos+4) / 4 - 1;
    byte bitOffset = (pos*2+8) & 7;
    boolean upperLimit = (bufferIndex+1) >= (XRES/4);
    const byte *character = charset[charIdx];
    for(byte y=0;y<8;y++) {
      byte charByte = character[y];
      byte highB = mono2HighColorByte(charByte) & channelMask;
      byte lowB = mono2LowColorByte(charByte) & channelMask;
      if (bufferIndex > 0) buffer[ bufferIndex ] |= highB >> bitOffset;
      if (!upperLimit) {
        if (bitOffset) buffer[ bufferIndex + 1 ] |= highB << (8-bitOffset);
        buffer[ bufferIndex+1 ] |= lowB >> bitOffset;
      }
      bufferIndex += XRES/4;
    }
  }
  pos += width+1;
}

inline signed char drawString(volatile byte *buffer, signed char pos, char *string, byte channelMask) {
#ifdef DEBUG
    Serial.println("drawString:");
#endif
  while(*string!=0 && pos < XRES) {
    byte charIdx = 10;
#ifdef DEBUG
    Serial.print(pos);
    Serial.print(":");
    Serial.print(*string);
    Serial.print(" ");
#endif
    if (*string >= '0' && *string <= '9')
      charIdx = *string - '0';
    drawChar( buffer, pos, charIdx, channelMask);
    string++;
  }
#ifdef DEBUG
    Serial.println();
#endif
  return pos;
}


void rowshift(boolean v);

inline void setupDisplay(void) {
  pinMode(COL_D, OUTPUT);
  pinMode(LINE_CLK, OUTPUT);
  pinMode(COL_OE, OUTPUT);
  pinMode(COL_ST, OUTPUT);
  digitalWrite(COL_OE, false);
  digitalWrite(COL_ST, false);
  pinMode(ROW_D, OUTPUT);
  pinMode(ROW_CP, OUTPUT);
  pinMode(ROW_OE, OUTPUT);
  pinMode(ROW_ST, OUTPUT);
  digitalWrite(ROW_OE, false);
  digitalWrite(ROW_ST, true);
  
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(LSBFIRST);
  for(int i = 0; i < 8; i++) {
    SPI.transfer(0);
    rowshift(true);
  }
  digitalWrite(ROW_OE, true);
}


inline void linedisable(void) {
  digitalWrite(COL_OE, false);
}

inline void lineenable(void) {
  digitalWrite(COL_OE, true);
}

inline void rowshift(boolean v) {
  if (v)
    digitalWrite(ROW_D, 1);
  else 
    digitalWrite(ROW_D, 0);
  digitalWrite(ROW_CP, 1);
  digitalWrite(ROW_CP, 0);
}  

inline void fillLineShift(void) {
  volatile byte *linebuffer = displayBuffer[ lineiters ] + line*XRES/4;
#ifdef DEBUG
    Serial.print("fillLine:");
    for(byte i=0; i < XRES/4; i ++) {
      Serial.print(linebuffer[i]);
      Serial.print(" ");
    }
    Serial.println();
#endif
#ifndef DEBUG
    for(byte i=0; i < XRES/4; i ++) {
      SPI.transfer(linebuffer[i]);
    }
#endif
}

inline void storeLine(boolean stat=true) {
#ifndef DEBUG
  digitalWrite(COL_ST, stat);
#endif
}

void displayCallback() {
#ifdef DEBUG
    Serial.println("displayCallback");
#endif
  linedisable();
  storeLine();
  if (!lineiters) rowshift(line != 0);
  lineenable();

  lineiters++;
  if (lineiters == 3) {
    lineiters = 0;
    line++; line &= 7;
  }
  storeLine(false);
  fillLineShift();
}

#endif // DISPLAY_H
