#ifndef CHARARCTER_H
#define CHARARCTER_H

#include <avr/io.h>
#include <avr/pgmspace.h>

extern const uint8_t _cosine[] PROGMEM;
const uint8_t _cosine[] = {255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 252, 251, 250, 249, 248, 247, 246, 245, 243, 242, 240, 239, 237, 236, 234, 232, 230, 228, 226, 224, 222, 220, 218, 216, 213, 211, 209, 206, 204, 201, 199, 196, 193, 191, 188, 185, 182, 179, 176, 174, 171, 168, 165, 162, 159, 156, 152, 149, 146, 143, 140, 137, 134, 131, 127, 124, 121, 118, 115, 112, 109, 106, 103, 99, 96, 93, 90, 87, 84, 81, 79, 76, 73, 70, 67, 64, 62, 59, 56, 54, 51, 49, 46, 44, 42, 39, 37, 35, 33, 31, 29, 27, 25, 23, 21, 19, 18, 16, 15, 13, 12, 10, 9, 8, 7, 6, 5, 4, 3, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 15, 16, 18, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 42, 44, 46, 49, 51, 54, 56, 59, 62, 64, 67, 70, 73, 76, 79, 81, 84, 87, 90, 93, 96, 99, 103, 106, 109, 112, 115, 118, 121, 124, 127, 131, 134, 137, 140, 143, 146, 149, 152, 156, 159, 162, 165, 168, 171, 174, 176, 179, 182, 185, 188, 191, 193, 196, 199, 201, 204, 206, 209, 211, 213, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 237, 239, 240, 242, 243, 245, 246, 247, 248, 249, 250, 251, 252, 252, 253, 254, 254, 255, 255, 255, 255, 255 };



#define i(c) ((uint8_t)(c==' '?0:1))
#define b(s,o) ((uint8_t)(\
  (i(s[0+o]) << 7) |\
  (i(s[1+o]) << 6) |\
  (i(s[2+o]) << 5) |\
  (i(s[3+o]) << 4) |\
  (i(s[4+o]) << 3)))
  
#define Character(s) { b(s,0),b(s,5),b(s,10),b(s,15),b(s,20),b(s,25),b(s,30),b(s,35) }

//const uint8_t _charset[][8] PROGMEM = {
const uint8_t _charset[][8]  = {
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

inline const uint8_t cosine(uint8_t x) { return pgm_read_byte(&(_cosine[x])); }
//inline const uint8_t charset(uint8_t c, uint8_t line) { return pgm_read_byte(&(_charset[c][line])); }
inline const uint8_t charset(uint8_t c, uint8_t line) { return _charset[c][line]; }

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

