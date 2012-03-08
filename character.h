#ifndef CHARARCTER_H
#define CHARARCTER_H

#include <avr/io.h>

static const uint8_t cosine[] = { 255,254,254,254,254,254,253,253,252,251,251,250,249,248,247,246,245,244,242,241,239,238,236,235,233,231,229,227,225,223,221,219,217,215,212,210,208,205,203,200,198,195,192,190,187,184,181,178,175,173,170,167,164,161,158,155,151,148,145,142,139,136,133,130,127,123,120,117,114,111,108,105,102,98,95,92,89,86,83,80,78,75,72,69,66,63,61,58,55,53,50,48,45,43,41,38,36,34,32,30,28,26,24,22,20,18,17,15,14,12,11,9,8,7,6,5,4,3,2,2,1,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,1,2,2,3,4,5,6,7,8,9,11,12,14,15,17,18,20,22,24,26,28,30,32,34,36,38,41,43,45,48,50,53,55,58,61,63,66,69,72,75,78,80,83,86,89,92,95,98,102,105,108,111,114,117,120,123,126,130,133,136,139,142,145,148,151,155,158,161,164,167,170,173,175,178,181,184,187,190,192,195,198,200,203,205,208,210,212,215,217,219,221,223,225,227,229,231,233,235,236,238,239,241,242,244,245,246,247,248,249,250,251,251,252,253,253,254,254,254,254,254 };

#define i(c) ((uint8_t)(c==' '?0:1))
#define b(s,o) ((uint8_t)(\
  (i(s[0+o]) << 7) |\
  (i(s[1+o]) << 6) |\
  (i(s[2+o]) << 5) |\
  (i(s[3+o]) << 4) |\
  (i(s[4+o]) << 3)))
  
#define Character(s) { b(s,0),b(s,5),b(s,10),b(s,15),b(s,20),b(s,25),b(s,30),b(s,35) }

static const uint8_t charset[][8]= {
Character( // 0 = 0
" @@@ "
"@   @"
"@  @@"
"@ @ @"
"@ @ @"
"@@  @"
"@   @"
" @@@ "
),
Character( // 1 = 1
"  X  "
" XX  "
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
),
Character( // 2 = 2
" @@@ "
"@   @"
"    @"
"   @ "
"  @  "
" @   "
"@    "
"@@@@@"
),
Character( // 3 = 3
" @@@ "
"@   @"
"    @"
"  @@ "
"    @"
"    @"
"@   @"
" @@@ "
),
Character( // 4 = 4
"   X "
"  XX "
" X X "
" X X "
"X  X "
"XXXXX"
"   X "
"   X "
),
Character( // 5 = 5
"@@@@@"
"@    "
"@    "
"@@@@ "
"    @"
"@   @"
"@   @"
" @@@ "
),
Character( // 6 = 6
" @@@ "
"@   @"
"@    "
"@@@@ "
"@   @"
"@   @"
"@   @"
" @@@ "
),
Character( // 7 = 7
"@@@@@"
"    @"
"   @ "
"   @ "
"  @  "
"  @  "
" @   "
" @   "
),
Character( // 8 = 8
" @@@ "
"@   @"
"@   @"
" @@@ "
"@   @"
"@   @"
"@   @"
" @@@ "
),
Character( // 9 = 9
" @@@ "
"@   @"
"@   @"
" @@@@"
"    @"
"    @"
"@   @"
" @@@ "
),
Character( // 10 = A
"XXXX "
"X   X"
"X   X"
"XXXXX"
"X   X"
"X   X"
"X   X"
"X   X"
),
Character( // 11 = B
"XXX  "
"X  X "
"X  X "
"XXXX "
"X   X"
"X   X"
"X   X"
"XXXX "
),
Character( // 12 = C
" XXX "
"X   X"
"X    "
"X    "
"X    "
"X    "
"X   X"
" XXX "
),
Character( // 13 = D
"XXX  "
"X  X "
"X   X"
"X   X"
"X   X"
"X   X"
"X   X"
"XXXX "
),
Character( // 14 = E
" XXXX"
"X    "
"X    "
"XXXX "
"X    "
"X    "
"X    "
"XXXXX"
),
Character( // 15 = F
" XXXX"
"X    "
"X    "
"XXX  "
"X    "
"X    "
"X    "
"X    "
),
Character( // 16 = G
"     "
"     "
"     "
"     "
"     "
"     "
"     "
"     "
),
Character( // 17 = H
"     "
"     "
"     "
"     "
"     "
"     "
"     "
"     "
),
Character( // 18 = I
"     "
"     "
"     "
"     "
"     "
"     "
"     "
"     "
),
Character( // 19 = J
"     "
"     "
"     "
"     "
"     "
"     "
"     "
"     "
),
Character( // 20 = K
"     "
"     "
"     "
"     "
"     "
"     "
"     "
"     "
),
Character( // 21 = L
"     "
"     "
"     "
"     "
"     "
"     "
"     "
"     "
),
Character( // 22 = M
"     "
"     "
"     "
"     "
"     "
"     "
"     "
"     "
),
Character( // 23 = N
"     "
"     "
"     "
"     "
"     "
"     "
"     "
"     "
),
Character( // 24 = O
"     "
"     "
"     "
"     "
"     "
"     "
"     "
"     "
),
Character( // 25 = P
"     "
"     "
"     "
"     "
"     "
"     "
"     "
"     "
),
Character( // 10 = invalid
"@@@@@"
"@   @"
"@@ @@"
"@ @ @"
"@ @ @"
"@@ @@"
"@   @"
"@@@@@"
),
};

#undef b
#undef i

#define mono2HighByte(c) ((uint8_t)\
  (c&0x80) | ((c>>1)&0x20) | ((c>>2)&0x08) | ((c>>3)&0x02)\
)

#define mono2LowByte(c) ((uint8_t)\
  (c&0x01) | ((c<<1)&0x04) | ((c<<2)&0x10) | ((c<<3)&0x40)\
)

#define mono2HighColorByte(c) ((uint8_t)mono2HighByte(c)|(mono2HighByte(c)>>1))
#define mono2LowColorByte(c) ((uint8_t)mono2LowByte(c)|(mono2LowByte(c)<<1))

#endif //CHARACTER_H

