( ************************************* )
( Layout:                               )
( $0164: USER CODE DEFAULT ENTRY        )
( ...                                   )
( $3000: USER HEAP START                )
( ...                                   )
( $7000: COMPILER CODE                  )
( ************************************* )
CONSTANT TIB      $64
CONSTANT F_IMME   128
CONSTANT TRUE    -1
CONSTANT FALSE    0
CONSTANT START-IP $0164
CONSTANT USR-HEAP $3000

VARIABLE POS
VARIABLE LAST
VARIABLE STEPPER
VARIABLE BASE

: 3DROP DROP DROP DROP ;
: 2DUP  OVER OVER ;
: ?DUP  DUP 0 <> IF DUP THEN ;

: CR 10 EMIT ;

: SPACE?
  DUP  9  =
  OVER 32 = OR
  OVER 10 = OR
  SWAP 13 = OR ;

: NON-SPACE? SPACE? INVERT ;

: POS++ POS @ 1 + POS ! ;
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

: PRINT
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

: >NFA 2 + ;
: >FFA >NFA DUP LENGTH + 1 + ;
: >CFA >FFA 1 + ;

: STEP  STEPPER @ @ STEPPER ! ;
: STEP? STEPPER @ 0 <> ;
: NFA   STEPPER @ >NFA ;

: IMMEDIATE? >FFA C@ F_IMME AND 0 <> ;

: IMMEDIATE
  LAST @ >FFA C@
  F_IMME OR 
  LAST @ >FFA C!

: FIND ( s -- addr / 0 )
  LAST @ STEPPER !
  BEGIN
    STEP?
  WHILE
    NFA OVER ( s ) STRING= IF
      DROP
      STEPPER @
      EXIT
    THEN
    STEP
  REPEAT
  DROP 0 ;

: BETWEEN? OVER >= -ROT <= AND ;

: MINUS? C@ 45 = ;
: HEXA?  C@ 36 = ;

: >DIGIT
  C@ 48 OVER 57 BETWEEN?
  IF 48 - ELSE 65 - 10 + THEN ;

: DIGIT?
  BASE @ 10 <= IF
    48 SWAP C@ 47 BASE @ + BETWEEN?
    EXIT
  THEN
  BASE @ 16 <= IF
    48 OVER C@ 57 BETWEEN?
    65 ROT  C@ 54 BASE @ + BETWEEN? OR
    EXIT
  THEN
  FALSE ;

: >NUMBER ( s -- n bool )
  DUP HEXA?  IF 1 + 16 ELSE 10 THEN BASE !
  DUP MINUS? IF 1 + -1 ELSE 1  THEN
  SWAP 0
  ( sign str result )
  BEGIN
    OVER NON-ZERO?
  WHILE
    OVER DIGIT? INVERT IF 3DROP FALSE EXIT THEN
    BASE @ * OVER >DIGIT +
    SWAP 1 + SWAP
  REPEAT
  NIP * TRUE ;

: CONVERT ( -- n bool ) TIB >NUMBER ;

: >OPCODE ( s -- opcode / 0 )
  DUP s" +"        STRING= IF DROP $01 EXIT THEN
  DUP s" -"        STRING= IF DROP $02 EXIT THEN
  DUP s" *"        STRING= IF DROP $03 EXIT THEN
  DUP s" /"        STRING= IF DROP $04 EXIT THEN
  DUP s" DUP"      STRING= IF DROP $05 EXIT THEN
  DUP s" DROP"     STRING= IF DROP $06 EXIT THEN
  DUP s" SWAP"     STRING= IF DROP $07 EXIT THEN
  DUP s" NIP"      STRING= IF DROP $08 EXIT THEN
  DUP s" OVER"     STRING= IF DROP $09 EXIT THEN
  DUP s" ROT"      STRING= IF DROP $0A EXIT THEN
  DUP s" -ROT"     STRING= IF DROP $0B EXIT THEN
  DUP s" TUCK"     STRING= IF DROP $0C EXIT THEN
  DUP s" INVERT"   STRING= IF DROP $0D EXIT THEN
  DUP s" AND"      STRING= IF DROP $0E EXIT THEN
  DUP s" OR"       STRING= IF DROP $0F EXIT THEN
  DUP s" XOR"      STRING= IF DROP $10 EXIT THEN
  DUP s" >"        STRING= IF DROP $11 EXIT THEN
  DUP s" >="       STRING= IF DROP $12 EXIT THEN
  DUP s" <"        STRING= IF DROP $13 EXIT THEN
  DUP s" <="       STRING= IF DROP $14 EXIT THEN
  DUP s" ="        STRING= IF DROP $15 EXIT THEN
  DUP s" <>"       STRING= IF DROP $16 EXIT THEN
  DUP s" JMP"      STRING= IF DROP $17 EXIT THEN
  DUP s" JZ"       STRING= IF DROP $18 EXIT THEN
  DUP s" JNZ"      STRING= IF DROP $19 EXIT THEN
  DUP s" AJMP"     STRING= IF DROP $1A EXIT THEN
  DUP s" CALL"     STRING= IF DROP $1B EXIT THEN
  DUP s" EXIT"     STRING= IF DROP $1C EXIT THEN
  DUP s" ."        STRING= IF DROP $1D EXIT THEN
  DUP s" NOP"      STRING= IF DROP $1E EXIT THEN
  DUP s" EMIT"     STRING= IF DROP $1F EXIT THEN
  DUP s" LIT"      STRING= IF DROP $20 EXIT THEN
  DUP s" %"        STRING= IF DROP $21 EXIT THEN
  DUP s" KEY"      STRING= IF DROP $22 EXIT THEN
  DUP s" SP"       STRING= IF DROP $23 EXIT THEN
  DUP s" SP!"      STRING= IF DROP $24 EXIT THEN
  DUP s" HALT"     STRING= IF DROP $25 EXIT THEN
  DUP s" LSHIFT"   STRING= IF DROP $26 EXIT THEN
  DUP s" RSHIFT"   STRING= IF DROP $27 EXIT THEN
  DUP s" !"        STRING= IF DROP $28 EXIT THEN
  DUP s" C!"       STRING= IF DROP $29 EXIT THEN
  DUP s" @"        STRING= IF DROP $2A EXIT THEN
  DUP s" C@"       STRING= IF DROP $2B EXIT THEN
  DUP s" DP"       STRING= IF DROP $2C EXIT THEN
  DUP s" DP!"      STRING= IF DROP $2D EXIT THEN
  DUP s" ,"        STRING= IF DROP $2E EXIT THEN
  DUP s" C,"       STRING= IF DROP $2F EXIT THEN
  DUP s" DEPTH"    STRING= IF DROP $30 EXIT THEN
  DUP s" >R"       STRING= IF DROP $31 EXIT THEN
  DUP s" R>"       STRING= IF DROP $32 EXIT THEN
  DUP s" EXEC"     STRING= IF DROP $33 EXIT THEN
  DUP s" DUMP"     STRING= IF DROP $34 EXIT THEN
  DUP s" ABORT"    STRING= IF DROP $35 EXIT THEN
  DUP s" [']"      STRING= IF DROP $36 EXIT THEN
  DROP 0
;

: FIND-PRIMITIVE ( -- opcode / 0 ) TIB >OPCODE ;

: ??? ( s -- ) TIB PRINT 32 EMIT 63 EMIT CR ;

: COMPILE ( -- )
  TIB FIND
  ?DUP IF
    DUP IMMEDIATE? IF
      >CFA EXEC
    ELSE
      ['] CALL C,
      >CFA , 
    THEN
  ELSE
    FIND-PRIMITIVE
    ?DUP IF
      ( OPCODE ) C,
    ELSE CONVERT
      IF
        ['] LIT C, ( NUM ) ,
      ELSE ??? THEN
    THEN
  THEN ;

: MAKE-HEADER ( name -- )
  DP
  LAST @ ,
  LAST !
  STRING, 
  0 ( FLAGS ) C, ;

: CREATE WORD MAKE-HEADER ;

: END ['] EXIT C, ;

: COMPILE-AJMP ['] AJMP C, START-IP , ;
: COMPILE-HALT ['] LIT  C, 0 , ['] HALT C, ;

: COMPILER-LOOP
  1 DP!
  COMPILE-AJMP
  START-IP DP!
  BEGIN
    WORD s" BYE" STRING= INVERT
  WHILE
    COMPILE
  REPEAT
  COMPILE-HALT ;

: DUMP-OUTPUT
  s" output.img" DUMP
  INVERT IF
    s" DUMP FAILED" PRINT CR
    ABORT
  THEN ;

: POST-CHECKS
  DEPTH 0 <> IF
    s" Non Empty stack: " PRINT DEPTH . CR
    ABORT
  THEN ;

ENTRY

( ***************** Dictionary Structure ***************** )
( Words:                                                   )
(  16b            8b 8b                                    )
(  LINK "<name1>" 00 FLAG INSTR.1 .. INSTR.N EXIT LINK ... )
(   ^---------------------------------------------+        )

USR-HEAP DP! ( <= User Dictionary Start )

0  LAST !
10 BASE !

s" :" MAKE-HEADER
  ['] CALL C, ' CREATE  ,
END IMMEDIATE

s" ;" MAKE-HEADER
  ['] LIT C, $1C , ( EXIT )
  ['] C, C,
END IMMEDIATE

s" IMMEDIATE" MAKE-HEADER
  ['] CALL C, ' IMMEDIATE ,
END IMMEDIATE

s" CREATE" MAKE-HEADER
  ['] CALL C, ' CREATE ,
END

s" ENTRY" MAKE-HEADER
  ['] DP  C, 
  ['] LIT C, $0002 , 
  ['] !   C,
END IMMEDIATE

COMPILER-LOOP
DUMP-OUTPUT
POST-CHECKS

0 HALT
