( *************************4*********** )
( Layout:                               )
( $0164: USER CODE DEFAULT ENTRY        )
( ...                                   )
( $3000: USER HEAP START                )
( ...                                   )
( $7000: COMPILER CODE                  )
( ************************************* )
: TIB      $64 ;
: F_IMME   128 ;
: TRUE      -1 ;
: FALSE      0 ;
: USR-HEAP $3000 ;

VARIABLE POS
VARIABLE LAST
VARIABLE STEPPER
VARIABLE BASE
VARIABLE TARGET

: SPACE?
  DUP  9  =
  OVER 32 = OR
  OVER 10 = OR
  SWAP 13 = OR ;

: POS++ POS @ 1+ POS ! ;
: STORE POS @ C! POS++ ;
: RESET TIB POS ! ;

: C= C@ SWAP C@ = ;

: STR=
  BEGIN
    2DUP C=
    OVER NON-ZERO? AND
    OVER NON-ZERO? AND
  WHILE
    1+ SWAP 1+
  REPEAT
  C= ;

: STRLEN
  0
  BEGIN
    OVER NON-ZERO?
  WHILE
    1+ SWAP
    1+ SWAP
  REPEAT
  NIP ;

: SKIP BEGIN KEY DUP SPACE? WHILE DROP REPEAT STORE ;

: WORD
  RESET SKIP
  BEGIN
    KEY DUP SPACE? INVERT
  WHILE
    STORE
  REPEAT
  DROP
  0 STORE
  TIB ;

: STR,
  BEGIN
    DUP NON-ZERO?
  WHILE
    DUP C@ C,
    1+
  REPEAT
  C@ C, ;

: >NFA 2 + ;
: >FFA >NFA DUP STRLEN + 1+ ;
: >CFA >FFA 1+ ;

: STEP  STEPPER @ @ STEPPER ! ;
: STEP? STEPPER @ 0 <> ;
: NFA   STEPPER @ >NFA ;

: IMMEDIATE? >FFA C@ F_IMME AND 0 <> ;

: IMMEDIATE
  LAST @ >FFA C@
  F_IMME OR
  LAST @ >FFA C! ;

: FIND ( s -- addr / 0 )
  LAST @ STEPPER !
  BEGIN
    STEP?
  WHILE
    NFA OVER ( s ) STR= IF
      DROP
      STEPPER @
      EXIT
    THEN
    STEP
  REPEAT
  DROP 0 ;

: NEG? C@ 45 = ;
: HEX? C@ 36 = ;

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
  DUP HEX? IF 1+ 16 ELSE 10 THEN BASE !
  DUP NEG? IF 1+ -1 ELSE 1  THEN
  SWAP 0
  ( sign str result )
  BEGIN
    OVER NON-ZERO?
  WHILE
    OVER DIGIT? INVERT IF 3DROP FALSE EXIT THEN
    BASE @ * OVER >DIGIT +
    SWAP 1+ SWAP
  REPEAT
  NIP * TRUE ;

: CONVERT ( -- n bool ) TIB >NUMBER ;

: >OPCODE ( s -- opcode / 0 )
  DUP s" +"        STR= IF DROP $01 EXIT THEN
  DUP s" -"        STR= IF DROP $02 EXIT THEN
  DUP s" *"        STR= IF DROP $03 EXIT THEN
  DUP s" /"        STR= IF DROP $04 EXIT THEN
  DUP s" 1+"       STR= IF DROP $05 EXIT THEN
  DUP s" 1-"       STR= IF DROP $06 EXIT THEN
  DUP s" DUP"      STR= IF DROP $07 EXIT THEN
  DUP s" DROP"     STR= IF DROP $08 EXIT THEN
  DUP s" SWAP"     STR= IF DROP $09 EXIT THEN
  DUP s" NIP"      STR= IF DROP $0A EXIT THEN
  DUP s" OVER"     STR= IF DROP $0B EXIT THEN
  DUP s" ROT"      STR= IF DROP $0C EXIT THEN
  DUP s" TUCK"     STR= IF DROP $0D EXIT THEN
  DUP s" INVERT"   STR= IF DROP $0E EXIT THEN
  DUP s" AND"      STR= IF DROP $0F EXIT THEN
  DUP s" OR"       STR= IF DROP $10 EXIT THEN
  DUP s" XOR"      STR= IF DROP $11 EXIT THEN
  DUP s" >"        STR= IF DROP $12 EXIT THEN
  DUP s" >="       STR= IF DROP $13 EXIT THEN
  DUP s" <"        STR= IF DROP $14 EXIT THEN
  DUP s" <="       STR= IF DROP $15 EXIT THEN
  DUP s" ="        STR= IF DROP $16 EXIT THEN
  DUP s" <>"       STR= IF DROP $17 EXIT THEN
  DUP s" JMP"      STR= IF DROP $18 EXIT THEN
  DUP s" JZ"       STR= IF DROP $19 EXIT THEN
  DUP s" JNZ"      STR= IF DROP $1A EXIT THEN
  DUP s" AJMP"     STR= IF DROP $1B EXIT THEN
  DUP s" CALL"     STR= IF DROP $1C EXIT THEN
  DUP s" EXIT"     STR= IF DROP $1D EXIT THEN
  DUP s" EMIT"     STR= IF DROP $1E EXIT THEN
  DUP s" LIT"      STR= IF DROP $1F EXIT THEN
  DUP s" %"        STR= IF DROP $20 EXIT THEN
  DUP s" KEY"      STR= IF DROP $21 EXIT THEN
  DUP s" SP"       STR= IF DROP $22 EXIT THEN
  DUP s" SP!"      STR= IF DROP $23 EXIT THEN
  DUP s" HALT"     STR= IF DROP $24 EXIT THEN
  DUP s" LSHIFT"   STR= IF DROP $25 EXIT THEN
  DUP s" RSHIFT"   STR= IF DROP $26 EXIT THEN
  DUP s" !"        STR= IF DROP $27 EXIT THEN
  DUP s" C!"       STR= IF DROP $28 EXIT THEN
  DUP s" @"        STR= IF DROP $29 EXIT THEN
  DUP s" C@"       STR= IF DROP $2A EXIT THEN
  DUP s" DP"       STR= IF DROP $2B EXIT THEN
  DUP s" DP!"      STR= IF DROP $2C EXIT THEN
  DUP s" >R"       STR= IF DROP $2D EXIT THEN
  DUP s" R>"       STR= IF DROP $2E EXIT THEN
  DUP s" R@"       STR= IF DROP $2F EXIT THEN
  DUP s" I"        STR= IF DROP $2F EXIT THEN \ Alias to R@
  DUP s" DUMP"     STR= IF DROP $30 EXIT THEN
  DUP s" ABORT"    STR= IF DROP $31 EXIT THEN
  DUP s" [']"      STR= IF DROP $32 EXIT THEN
  DUP s" '"        STR= IF DROP $33 EXIT THEN
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
  STR,
  0 ( FLAGS ) C, ;

: CREATE WORD MAKE-HEADER ;

: END ['] EXIT C, ;

: COMPILE-AJMP ['] AJMP C, TARGET @ , ;
: COMPILE-HALT ['] LIT  C, 0 , ['] HALT C, ;

: COMPILER-LOOP
  1 DP!
  COMPILE-AJMP
  TARGET @ DP!
  BEGIN
    WORD s" BYE" STR= INVERT
  WHILE
    COMPILE
  REPEAT
  COMPILE-HALT ;

: DUMP-OUTPUT
  s" output.img" DUMP
  INVERT IF
    s" Dump failed." PRINT CR
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

\ Same source code is used for stage1 and stage2 compiler.
\ Use different target address depending the stage.
STEPPER $7000 >= IF
  $6000 TARGET ! \ We're in stage1 compiler
ELSE
  $0164 TARGET ! \ We're in stage2 compiler
THEN

USR-HEAP DP! ( <= User Dictionary Start )

0  LAST !
10 BASE !

s" :" MAKE-HEADER
  ['] CALL C, ' CREATE  ,
END IMMEDIATE

s" ;" MAKE-HEADER
  ['] LIT  C, ['] EXIT ,
  ['] CALL C,  '  C,   ,  \ C, is not a primitive
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

BYE
