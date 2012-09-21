#include "character.h"

const uint8_t _cosine[] = {255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 252, 251, 250, 249, 248, 247, 246, 245, 243, 242, 240, 239, 237, 236, 234, 232, 230, 228, 226, 224, 222, 220, 218, 216, 213, 211, 209, 206, 204, 201, 199, 196, 193, 191, 188, 185, 182, 179, 176, 174, 171, 168, 165, 162, 159, 156, 152, 149, 146, 143, 140, 137, 134, 131, 127, 124, 121, 118, 115, 112, 109, 106, 103, 99, 96, 93, 90, 87, 84, 81, 79, 76, 73, 70, 67, 64, 62, 59, 56, 54, 51, 49, 46, 44, 42, 39, 37, 35, 33, 31, 29, 27, 25, 23, 21, 19, 18, 16, 15, 13, 12, 10, 9, 8, 7, 6, 5, 4, 3, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 15, 16, 18, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 42, 44, 46, 49, 51, 54, 56, 59, 62, 64, 67, 70, 73, 76, 79, 81, 84, 87, 90, 93, 96, 99, 103, 106, 109, 112, 115, 118, 121, 124, 127, 131, 134, 137, 140, 143, 146, 149, 152, 156, 159, 162, 165, 168, 171, 174, 176, 179, 182, 185, 188, 191, 193, 196, 199, 201, 204, 206, 209, 211, 213, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 237, 239, 240, 242, 243, 245, 246, 247, 248, 249, 250, 251, 252, 252, 253, 254, 254, 255, 255, 255, 255, 255 };



#define i(c) ((uint8_t)(c==' '?0:1))
#define b(s,o) ((uint8_t)(\
  (i(s[0+o]) << 7) |\
  (i(s[1+o]) << 6) |\
  (i(s[2+o]) << 5) |\
  (i(s[3+o]) << 4) |\
  (i(s[4+o]) << 3)))
  
#define Character(s) b(s,0),b(s,5),b(s,10),b(s,15),b(s,20),b(s,25),b(s,30),b(s,35)

const uint8_t _charset[] = {
Character( // 32 = SP
"     "
"     "
"     "
"     "
"     "
"     "
"     "
"     "
)};/*,
Character( // 33 = !
" @@  "
" @@  "
" @@  "
" @@  "
" @@  "
"     "
" @@  "
" @@  "
),
Character( // 34 = "
" @ @ "
" @ @ "
"     "
"     "
"     "
"     "
"     "
"     "
),
Character( // 35 = #
"     "
" @ @ "
"@@@@@"
" @ @ "
"@@@@@"
" @ @ "
"     "
"     "
),
Character( // 36 = $
"  @  "
" @@@@"
"@ @  "
" @@@ "
"  @ @"
"  @ @"
"@@@@ "
"  @  "
),
Character( // 37 = %
"@@  @"
"@@ @ "
"   @ "
"  @  "
"  @  "
" @   "
" @ @@"
"@  @@"
),
Character( // 38 = &
"     "
" XXX "
"X   X"
"@  X "
" @X  "
"@ @ @"
"@  @ "
" @@ @"
),
Character( // 39 = '
"  @  "
"  @  "
"     "
"     "
"     "
"     "
"     "
"     "
),
Character( // 40 = (
"   @X"
"  @  "
"  @  "
" @   "
" @   "
"  @  "
"  @  "
"   @X"
),
Character( // 41 = )
"X@   "
"  @  "
"  @  "
"   @ "
"   @ "
"  @  "
"  @  "
"X@   "
),
Character( // 42 = *
"     "
"  @  "
"@ @ @"
" @@@ "
" @ @ "
"@   @"
"     "
"     "
),
Character( // 43 = +
"     "
"  @  "
"  @  "
"@@@@@"
"  @  "
"  @  "
"     "
"     "
),
Character( // 44 = ,
"     "
"     "
"     "
"     "
"     "
"  @@ "
"  @@ "
"   @ "
),
Character( // 45 = -
"     "
"     "
"     "
"@@@@@"
"     "
"     "
"     "
"     "
),
Character( // 46 = .
"     "
"     "
"     "
"     "
"     "
"     "
" @@  "
" @@  "
),
Character( // 47 = /
"    @"
"   @ "
"   @ "
"  @  "
"  @  "
" @   "
" @   "
"@    "
),
Character( // 48 = 0
" @@@ "
"@   @"
"@  @@"
"@ @ @"
"@ @ @"
"@@  @"
"@   @"
" @@@ "
),
Character( // 49 = 1
"  X  "
" XX  "
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
),
Character( // 50 = 2
" @@@ "
"@   @"
"    @"
"   @ "
"  @  "
" @   "
"@    "
"@@@@@"
),
Character( // 51 = 3
" @@@ "
"@   @"
"    @"
"  @@ "
"    @"
"    @"
"@   @"
" @@@ "
),
Character( // 52 = 4
"   X "
"  XX "
" X X "
" X X "
"X  X "
"XXXXX"
"   X "
"   X "
),
Character( // 53 = 5
"@@@@@"
"@    "
"@    "
"@@@@ "
"    @"
"@   @"
"@   @"
" @@@ "
),
Character( // 54 = 6
" @@@ "
"@   @"
"@    "
"@@@@ "
"@   @"
"@   @"
"@   @"
" @@@ "
),
Character( // 55 = 7
"@@@@@"
"    @"
"   @ "
"   @ "
"  @  "
"  @  "
" @   "
" @   "
),
Character( // 56 = 8
" @@@ "
"@   @"
"@   @"
" @@@ "
"@   @"
"@   @"
"@   @"
" @@@ "
),
Character( // 57 = 9
" @@@ "
"@   @"
"@   @"
" @@@@"
"    @"
"    @"
"@   @"
" @@@ "
),
Character( // 58 = :
"     "
"     "
" @@  "
" @@  "
"     "
"     "
" @@  "
" @@  "
),
Character( // 59 = ;
"     "
"     "
" @@  "
" @@  "
"     "
" @@  "
" @@  "
"  @  "
),
Character( // 60 = <
"    @"
"   @ "
"  @  "
" @   "
"  @  "
"   @ "
"    @"
"     "
),
Character( // 61 = =
"     "
"     "
"@@@@@"
"     "
"@@@@@"
"     "
"     "
"     "
),
Character( // 62 = >
"@    "
" @   "
"  @  "
"   @ "
"  @  "
" @   "
"@    "
"     "
),
Character( // 63 = ?
"@@@@ "
"    @"
"    @"
"  @@ "
" @@  "
"     "
" @@  "
" @@  "
),
Character( // 64 = @
" @@@ "
"@   @"
"@ @ @"
"@ @ @"
"@ @ @"
"@ @@@"
"@    "
" @@@ "
),
Character( // 65 = A
"XXXX "
"X   X"
"X   X"
"XXXXX"
"X   X"
"X   X"
"X   X"
"X   X"
),
Character( // 66 = B
"XXX  "
"X  X "
"X  X "
"XXXX "
"X   X"
"X   X"
"X   X"
"XXXX "
),
Character( // 67 = C
" XXX "
"X   X"
"X    "
"X    "
"X    "
"X    "
"X   X"
" XXX "
),
Character( // 68 = D
"XXX  "
"X  X "
"X   X"
"X   X"
"X   X"
"X   X"
"X   X"
"XXXX "
),
Character( // 69 = E
" XXXX"
"X    "
"X    "
"XXXX "
"X    "
"X    "
"X    "
"XXXXX"
),
Character( // 70 = F
" XXXX"
"X    "
"X    "
"XXX  "
"X    "
"X    "
"X    "
"X    "
),
Character( // 71 = G
" XXXX"
"X    "
"X    "
"X XXX"
"X   X"
"X   X"
"X   X"
" XXX "
),
Character( // 72 = H
"X   X"
"X   X"
"X   X"
"XXXXX"
"X   X"
"X   X"
"X   X"
"X   X"
),
Character( // 73 = I
"XXXXX"
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
"XXXXX"
),
Character( // 74 = J
"XXXX "
"    X"
"    X"
"  XXX"
"    X"
"X   X"
"X   X"
" XXX "
),
Character( // 75 = K
"X   X"
"X  X "
"X X  "
"XXX  "
"X X  "
"X  X "
"X  X "
"X   X"
),
Character( // 76 = L
"X    "
"X    "
"X    "
"X    "
"X    "
"X    "
"X    "
"XXXXX"
),
Character( // 77 = M
"X   X"
"XX XX"
"X X X"
"X X X"
"X   X"
"X   X"
"X   X"
"X   X"
),
Character( // 78 = N
"X   X"
"XX  X"
"X X X"
"X X X"
"X X X"
"X  XX"
"X   X"
"X   X"
),
Character( // 79 = O
" XXX "
"X   X"
"X   X"
"X   X"
"X   X"
"X   X"
"X   X"
" XXX "
),
Character( // 80 = P
"XXXX "
"X   X"
"X   X"
"XXXX "
"X    "
"X    "
"X    "
"X    "
),
Character( // 81 = Q
" XXX "
"X   X"
"X   X"
"X   X"
"X   X"
"X X X"
"X  X "
" XX X"
),
Character( // 82 = R
"XXXX "
"X   X"
"X   X"
"XXXX "
"X   X"
"X   X"
"X   X"
"X   X"
),
Character( // 83 = S
" XXX "
"X   X"
"X    "
"XXXX "
"    X"
"    X"
"X   X"
" XXX "
),
Character( // 84 = T
"XXXXX"
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
),
Character( // 85 = U
"X   X"
"X   X"
"X   X"
"X   X"
"X   X"
"X   X"
"X   X"
" XXX "
),
Character( // 86 = V
"X   X"
"X   X"
"X   X"
" X X "
" X X "
" X X "
"  X  "
"  X  "
),
Character( // 87 = W
"X   X"
"X   X"
"X   X"
"X   X"
"X X X"
"X X X"
"XX XX"
"X   X"
),
Character( // 88 = X
"X   X"
"X   X"
" X X "
"  X  "
" X X "
" X X "
"X   X"
"X   X"
),
Character( // 89 = Y
"X   X"
"X   X"
"X   X"
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
),
Character( // 90 = Z
"XXXXX"
"    X"
"   X "
" XXXX"
"  X  "
" X   "
"X    "
"XXXXX"
),
Character( // 91 = [
"  @@@"
"  @  "
"  @  "
"  @  "
"  @  "
"  @  "
"  @  "
"  @@@"
),
Character( // 92 = Backslash
"@    "
" @   "
" @   "
"  @  "
"  @  "
"   @ "
"   @ "
"    @"
),
Character( // 93 = ]
"@@@  "
"  @  "
"  @  "
"  @  "
"  @  "
"  @  "
"  @  "
"@@@  "
),
Character( // 94 = ^
"  @  "
" @ @ "
"@   @"
"     "
"     "
"     "
"     "
"     "
),
Character( // 95 = _
"     "
"     "
"     "
"     "
"     "
"     "
"     "
"@@@@@"
),
Character( // 96 = '
" @   "
" @   "
"  @  "
"     "
"     "
"     "
"     "
"     "
),
Character( // 97 = a
"     "
" XXX "
"X   X"
"    X"
" XXXX"
"X   X"
"X   X"
" XXXX"
),
Character( // 98 = b
"X    "
"X    "
"X    "
"XXXX "
"X   X"
"X   X"
"X   X"
"XXXX "
),
Character( // 99 = c
"     "
"     "
" XXX "
"X   X"
"X    "
"X    "
"X   X"
" XXX "
),
Character( // 100 = d
"    X"
"    X"
"    X"
" XXXX"
"X   X"
"X   X"
"X   X"
" XXXX"
),
Character( // 101 = e
"     "
"     "
" XXX "
"X   X"
"X   X"
"XXXX "
"X    "
" XXXX"
),
Character( // 102 = f
"  XX "
" X  X"
" X   "
"XXX  "
" X   "
" X   "
" X   "
" X   "
),
Character( // 103 = g
"     "
" XXXX"
"X   X"
"X   X"
" XXXX"
"    X"
"X   X"
" XXX "
),
Character( // 104 = h
"X    "
"X    "
"X    "
"XXXX "
"X   X"
"X   X"
"X   X"
"X   X"
),
Character( // 105 = i
"  X  "
"     "
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
),
Character( // 106 = j
"   X "
"     "
"   X "
"   X "
"   X "
"   X "
"X  X "
" XX  "
),
Character( // 107 = k
"X    "
"X    "
"X    "
"X   X"
"X XX "
"XX   "
"X XX "
"X   X"
),
Character( // 108 = l
"XX   "
" X   "
" X   "
" X   "
" X   "
" X   "
" X   "
"  XXX"
),
Character( // 109 = m
"     "
"     "
"XX X "
"X X X"
"X X X"
"X X X"
"X X X"
"X X X"
),
Character( // 110 = n
"     "
"     "
"X XX "
"XX  X"
"X   X"
"X   X"
"X   X"
"X   X"
),
Character( // 111 = o
"     "
"     "
" XXX "
"X   X"
"X   X"
"X   X"
"X   X"
" XXX "
),
Character( // 112 = p
"     "
" XXX "
"X   X"
"X   X"
"XXXX "
"X    "
"X    "
"X    "
),
Character( // 113 = q
"     "
" XXXX"
"X   X"
"X   X"
" XXXX"
"    X"
"    X"
"    X"
),
Character( // 114 = r
"     "
"     "
"X  XX"
"XXX  "
"X    "
"X    "
"X    "
"X    "
),
Character( // 115 = s
"     "
"     "
" XXXX"
"X    "
" XXX "
"    X"
"    X"
"XXXX "
),
Character( // 116 = t
"  X  "
"  X  "
"XXXXX"
"  X  "
"  X  "
"  X  "
"  X  "
"  XX "
),
Character( // 117 = u
"     "
"     "
"X   X"
"X   X"
"X   X"
"X   X"
"X   X"
" XXX "
),
Character( // 118 = v
"     "
"     "
"X   X"
"X   X"
"X   X"
" X X "
" X X "
"  X  "
),
Character( // 119 = w
"     "
"     "
"X X X"
"X X X"
"X X X"
" X X "
" X X "
" X X "
),
Character( // 120 = x
"     "
"     "
"X   X"
"X   X"
" X X "
"  X  "
" X X "
"X   X"
),
Character( // 121 = y
"     "
"     "
"X   X"
"X   X"
" XXX "
"   X "
"  X  "
"XX   "
),
Character( // 122 = z
"     "
"     "
"XXXXX"
"    X"
" XXXX"
"  X  "
" X   "
"XXXXX"
),
Character( // 123 = {
"  XXX"
" X   "
" X   "
"X    "
" X   "
" X   "
" X   "
"  XXX"
),
Character( // 124 = |
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
"  X  "
),
Character( // 125 = }
"XXX  "
"   X "
"   X "
"    X"
"   X "
"   X "
"   X "
"XXX  "
),
Character( // 126 = ~
"     "
"     "
"     "
" XX X"
"X  X "
"     "
"     "
"     "
),
Character( // 127 = invalid
"@@@@@"
"@   @"
"@@ @@"
"@ @ @"
"@ @ @"
"@@ @@"
"@   @"
"@@@@@"
)
};
*/
/*
uint8_t cosine(uint8_t x) { return pgm_read_byte(&(_cosine[x])); }
//inline const uint8_t cosine(uint8_t x) { return _cosine[x]; }
//inline const uint8_t charset(uint8_t c, uint8_t line) { return pgm_read_byte(&(_charset[(c<<3)+line])); }
uint8_t charset(uint8_t c, uint8_t line) { return 0xff;}//return _charset[(c<<3)+line]; }
*/
