CONSTANT TIB      0x64
CONSTANT F_PRIM   0b01000000
CONSTANT F_IMME   0b10000000
CONSTANT MASK_XT  0b00111111
CONSTANT TRUE    -1
CONSTANT FALSE    0

VARIABLE POS
VARIABLE LAST
VARIABLE STEPPER

0 LAST !

: 2DROP DROP DROP ;
: 3DROP DROP DROP DROP ;
: 2DUP OVER OVER ;
: CR 10 EMIT ;

: ?DUP DUP 0 <> IF DUP THEN ;

: SPACE?
  DUP  9  =
  OVER 32 = OR
  OVER 10 = OR
  SWAP 13 = OR ;

: NON-SPACE? SPACE? INVERT ;

: POS++ POS @ 1 + POS ! ;
: LEN   POS @ TIB - ;
: STORE POS @ C! POS++ ;
: RESET TIB POS ! ;

: CHR= C@ SWAP C@ = ;
: NON-ZERO? C@ 0 <> ;

: STRING=
  BEGIN
    2DUP CHR= 
    OVER NON-ZERO? AND
    OVER NON-ZERO? AND
  WHILE
    1 + SWAP 1 +
  REPEAT
  CHR= ;

: LENGTH
  0
  BEGIN
    OVER NON-ZERO?
  WHILE
    1 + SWAP
    1 + SWAP
  REPEAT
  NIP ;

: TYPE
  BEGIN
    DUP NON-ZERO?
  WHILE
    DUP C@ EMIT
    1 +
  REPEAT
  DROP ;

: SKIP BEGIN KEY DUP SPACE? WHILE DROP REPEAT STORE ;

: WORD
  RESET SKIP
  BEGIN
    KEY DUP NON-SPACE?
  WHILE
    STORE
  REPEAT
  DROP
  0 STORE
  TIB ;

: STRING,
  BEGIN
    DUP NON-ZERO?
  WHILE
    DUP C@ C,
    1 +
  REPEAT
  C@ C, ;
  
: STEP  STEPPER @ @ STEPPER ! ;
: STEP? STEPPER @ 0 <> ;

: >NAME   STEPPER @ 3 + ;
: >OPCODE MASK_XT AND ; 

: PRIMITIVE? ( n -- bool ) F_PRIM AND 0 <> ;

: FIND ( s -- addr / 0 )
  LAST @ STEPPER !
  BEGIN
    STEP?
  WHILE
    >NAME OVER STRING= IF
      DROP
      ( return addr. of FLAG/OPCODE  )
      STEPPER @ 2 + 
      EXIT
    THEN
    STEP
  REPEAT
  DROP 0 ;

( TODO: validate 0..9 )

: DIGIT     C@ 48 - ;
: NONDIGIT? C@ DUP 48 < SWAP 57 > OR ;
: MINUS?    C@ 45 = ;

: >NUMBER ( s -- n TRUE / FALSE )
  DUP MINUS? IF 1 + -1 ELSE 1 THEN
  SWAP 0
  ( sign str result )
  BEGIN
    OVER NON-ZERO?
  WHILE
    OVER NONDIGIT? IF 3DROP FALSE EXIT THEN
    10 * OVER DIGIT +
    SWAP 1 + SWAP
  REPEAT
  NIP * TRUE ;

: LIT, ( n -- ) s" LIT" FIND @ >OPCODE , , ;
: UNKNOWN ( s -- ) TYPE 32 EMIT 63 EMIT ;

: COMPILE
  WORD DUP FIND
  ?DUP IF
    DUP @ PRIMITIVE? IF
      >OPCODE , DROP
    ELSE
      s" CALL" FIND @ >OPCODE ,
      1 + , ( Word Start )
    THEN
  ELSE
    DUP >NUMBER
    IF
      LIT, DROP
    ELSE
      UNKNOWN
    THEN
  THEN ;

: DEF-PRIMITIVE ( name opcode -- )
  DP
  LAST @ , 
  LAST !
  F_PRIM OR C, 
  STRING, ;
  
: DEF-WORD ( name imm? -- )
  DP
  LAST @ , 
  LAST !
  IF F_IMME ELSE 0 THEN C,
  STRING, ;

: END-WORD s" RET" FIND @ >OPCODE , ;

ENTRY

( TODO )
0x800 DP!

( **************** Dictionary Structure **************** )
( Primitives:                                            )
(  [16b]         [8b] [8b]    [16b]                      )
(  LINK "<name1>" 00 F_OPCODE LINK "<name2>" 00 F_OPCODE )
(   ^--------------------------+                         )
( Words:                                                 )
(  LINK "<name1>" 00 FLAG INSTR.1 .. INSTR.N RET LINK .. )
(   ^---------------------------------------------+      )

s" +"           0x01   DEF-PRIMITIVE
s" -"           0x02   DEF-PRIMITIVE
s" *"           0x03   DEF-PRIMITIVE
s" /"           0x04   DEF-PRIMITIVE
s" DUP"         0x05   DEF-PRIMITIVE
s" DROP"        0x06   DEF-PRIMITIVE
s" SWAP"        0x07   DEF-PRIMITIVE
s" NIP"         0x08   DEF-PRIMITIVE
s" OVER"        0x09   DEF-PRIMITIVE
s" ROT"         0x0A   DEF-PRIMITIVE
s" -ROT"        0x0B   DEF-PRIMITIVE
s" TUCK"        0x0C   DEF-PRIMITIVE
s" INVERT"      0x0D   DEF-PRIMITIVE
s" AND"         0x0E   DEF-PRIMITIVE
s" OR"          0x0F   DEF-PRIMITIVE
s" XOR"         0x10   DEF-PRIMITIVE
s" >"           0x11   DEF-PRIMITIVE
s" >="          0x12   DEF-PRIMITIVE
s" <"           0x13   DEF-PRIMITIVE
s" <="          0x14   DEF-PRIMITIVE
s" ="           0x15   DEF-PRIMITIVE
s" <>"          0x16   DEF-PRIMITIVE
s" JMP"         0x17   DEF-PRIMITIVE
s" JZ"          0x18   DEF-PRIMITIVE
s" JNZ"         0x19   DEF-PRIMITIVE
s" AJMP"        0x1A   DEF-PRIMITIVE
s" CALL"        0x1B   DEF-PRIMITIVE
s" EXIT"        0x1C   DEF-PRIMITIVE
s" ."           0x1D   DEF-PRIMITIVE
s" NOP"         0x1E   DEF-PRIMITIVE
s" EMIT"        0x1F   DEF-PRIMITIVE
s" LIT"         0x20   DEF-PRIMITIVE
s" %"           0x21   DEF-PRIMITIVE
s" KEY"         0x22   DEF-PRIMITIVE
s" SP"          0x23   DEF-PRIMITIVE
s" SP!"         0x24   DEF-PRIMITIVE
s" HALT"        0x25   DEF-PRIMITIVE
s" LSHIFT"      0x26   DEF-PRIMITIVE
s" RSHIFT"      0x27   DEF-PRIMITIVE
s" !"           0x28   DEF-PRIMITIVE
s" C!"          0x29   DEF-PRIMITIVE
s" @"           0x2A   DEF-PRIMITIVE
s" C@"          0x2B   DEF-PRIMITIVE
s" DP"          0x2C   DEF-PRIMITIVE
s" DP!"         0x2D   DEF-PRIMITIVE
s" ,"           0x2E   DEF-PRIMITIVE
s" C,"          0x2F   DEF-PRIMITIVE
s" DEPTH"       0x30   DEF-PRIMITIVE
s" >R"          0x31   DEF-PRIMITIVE
s" R>"          0x32   DEF-PRIMITIVE

s" SQUARE" FALSE DEF-WORD
s" DUP" FIND @ >OPCODE ,
s" *"   FIND @ >OPCODE ,
END-WORD

0x2000 DP!

BEGIN
  COMPILE
AGAIN

CR DEPTH . CR

0 HALT
