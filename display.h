#ifndef DISPLAY_H
#define DISPLAY_H

#define XRES 8
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

volatile byte displayBuff[3 * XRES * 2];
volatile byte drawBuff[3 * XRES * 2];
volatile uint8_t *displayBuffer = displayBuff;
volatile uint8_t *drawBuffer = drawBuff;
volatile uint8_t switchBuffersFlag=0;

volatile byte line = 255;
volatile byte lineiters = 0;

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

void getCopperBars(uint8_t *color, uint16_t t) {
  for(int y=0;y<8;y++) {
    uint8_t mask = 0;
    uint16_t tg = (-t+y*7)*7;
    uint16_t tr = (t+y*9)*5;
    uint8_t vg = cosine[ tg & 255 ]>>6;
    uint8_t vr = cosine[ tr & 255 ]>>6;
    mask |= (vg<<2) | vr;
    color[y] = mask;
  }
}

void colorBar(volatile uint8_t *buffer, const uint8_t *color) { // higher bits = background, lower bits = foreground
  for(int8_t i=2; i>=0; i--) {
    volatile uint8_t *lookupbuffer = buffer;
    volatile uint8_t *fillbuffer = &(buffer[XRES*2*i]);
    for(uint8_t y=0; y < 8; y++) {
      uint8_t col = color[y];
      uint8_t bgmask = 0, fgmask = 0;
      if ((col >> 6) >  i) bgmask |= greenMask;
      if (((col >> 4)&3) >  i) bgmask |= redMask;
      if (((col >> 2)&3) >  i) fgmask |= greenMask;
      if (((col)&3) >  i) fgmask |= redMask;
      for(uint8_t x=0; x < XRES/4; x++) {
        uint8_t val = lookupbuffer[x];
        fillbuffer[x] = (val & fgmask) | ((~val)&bgmask);
      }
      lookupbuffer+=XRES/4;
      fillbuffer+=XRES/4;
    }
  }
}


inline void clearBuffer(volatile byte *buffer) {
  for(byte i=0;i < XRES * 2;i++)
    buffer[i]=0;
}

inline void drawChar(volatile byte *buffer, signed char &pos, byte charIdx) {
#ifdef DEBUG
  Serial.print("drawChar:"); Serial.println(pos);
#endif
  const signed char width = 5;
  if ((pos > -width) && (pos < XRES)) {
    signed char bufferIndex = ((pos+4) / 4) - 1;
    byte bitOffset = (pos*2+8) & 7;
    boolean upperLimit = !((bufferIndex+1) >= (XRES/4));
    boolean lowerLimit = (bufferIndex >= 0);
    const byte *character = charset[charIdx];
#ifdef DEBUG
  Serial.print("bufferIndex:"); Serial.println(bufferIndex);
#endif
    for(byte y=0;y<8;y++) {
      byte charByte = character[y];
      byte highB = mono2HighColorByte(charByte);
      byte lowB = mono2LowColorByte(charByte);
      if (lowerLimit) buffer[ bufferIndex ] |= highB >> bitOffset;
      if (upperLimit) {
        if (bitOffset) buffer[ bufferIndex + 1 ] |= highB << (8-bitOffset);
        buffer[ bufferIndex+1 ] |= lowB >> bitOffset;
      }
      bufferIndex += XRES/4;
    }
  }
  pos += width+1;
}

inline signed char drawString(volatile byte *buffer, signed char pos, const char *string) {
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
    drawChar( buffer, pos, charIdx);
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
  volatile byte *linebuffer = &(displayBuffer[ lineiters * XRES*2 + line*XRES/4]);
#ifndef DEBUG
    byte i=XRES/4;
    do {
      i--;
      SPI.transfer(linebuffer[i]);
    } while(i);
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
    line++; 
    if ( line == 8 ) {
      line = 0;
      if (switchBuffersFlag) {
        volatile byte *t = drawBuffer;
        drawBuffer = displayBuffer;
        displayBuffer = t;
        switchBuffersFlag = 0;
      }
    }
  }
  storeLine(false);
  fillLineShift();
}

#endif // DISPLAY_H
