( ************************************* )
( Layout:                               )
( $0182: USER MAIN ENTRY [JUMP TO] +    )
( $0200: USER DICTIONARY START     |    )
( ...                              |    )
( ... CODE START  <----------------+    )
( ...                                   )
( $6000: STAGE2 COMPILER RESIDUAL       )
( $7000: STAGE1 COMPILER RESIDUAL       )
( ************************************* )
: TIB     $102 ;
: F_IMME  %10000000 ;

: MAX-LINE-LEN 127 ;

: MAIN          $0190 ;
: STAGE1-TARGET $7000 ;
: STAGE2-TARGET $0200 ;
: VMPARAM-ADDR  $0042 ;

VARIABLE POS
VARIABLE LAST
VARIABLE STEPPER
VARIABLE BASE

: SPACE?
  DUP  9  =
  OVER 32 = OR
  OVER 10 = OR
  SWAP 13 = OR ;

: RESET  TIB POS ! ;
: TIBLEN POS @ TIB - ;
: STORE
  TIBLEN MAX-LINE-LEN > IF
    " Input buffer overflow." ABORT
  THEN
  POS @ C! POS ++ ;

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
: BIN? C@ 37 = ;

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
  DUP HEX? IF
    1+ 16
  ELSE
    DUP BIN? IF 1+ 2 ELSE 10 THEN
  THEN
  BASE !
  DUP NEG? IF 1+ -1 ELSE 1 THEN
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

: CHECK-OPCODE
  -ROT OVER STR= IF
    DROP
    R> DROP
    EXIT
  ELSE NIP
  THEN ;

: >OPCODE ( s -- opcode / 0 )
  " +"      $01   CHECK-OPCODE
  " -"      $02   CHECK-OPCODE
  " *"      $03   CHECK-OPCODE
  " /"      $04   CHECK-OPCODE
  " 1+"     $05   CHECK-OPCODE
  " 1-"     $06   CHECK-OPCODE
  " DUP"    $07   CHECK-OPCODE
  " DROP"   $08   CHECK-OPCODE
  " SWAP"   $09   CHECK-OPCODE
  " NIP"    $0A   CHECK-OPCODE
  " OVER"   $0B   CHECK-OPCODE
  " ROT"    $0C   CHECK-OPCODE
  " INVERT" $0D   CHECK-OPCODE
  " AND"    $0E   CHECK-OPCODE
  " OR"     $0F   CHECK-OPCODE
  " XOR"    $10   CHECK-OPCODE
  " >"      $11   CHECK-OPCODE
  " >="     $12   CHECK-OPCODE
  " <"      $13   CHECK-OPCODE
  " <="     $14   CHECK-OPCODE
  " ="      $15   CHECK-OPCODE
  " <>"     $16   CHECK-OPCODE
  " JMP"    $17   CHECK-OPCODE
  " JZ"     $18   CHECK-OPCODE
  " CALL"   $19   CHECK-OPCODE
  " IN"     $1A   CHECK-OPCODE
  " OUT"    $1B   CHECK-OPCODE
  " LIT"    $1C   CHECK-OPCODE
  " %"      $1D   CHECK-OPCODE
  " EXIT"   $1E   CHECK-OPCODE
  " SP"     $1F   CHECK-OPCODE
  " HALT"   $20   CHECK-OPCODE
  " LSHIFT" $21   CHECK-OPCODE
  " RSHIFT" $22   CHECK-OPCODE
  " !"      $23   CHECK-OPCODE
  " @"      $24   CHECK-OPCODE
  " >R"     $25   CHECK-OPCODE
  " R>"     $26   CHECK-OPCODE
  " R@"     $27   CHECK-OPCODE
  " I"      $27   CHECK-OPCODE \ Alias to R@
  " DUMP"   $28   CHECK-OPCODE
  " ABORT"  $29   CHECK-OPCODE
  DROP 0
;

: FIND-PRIMITIVE ( -- opcode / 0 ) TIB >OPCODE ;

: ??? ( s -- )
  TIB PRINT "  ? - Unknown word." PRINT CR ;

: COMPILE ( -- )
  TIB FIND
  ?DUP IF
    DUP IMMEDIATE? IF
      >CFA EXEC
    ELSE
      CODE CALL >CFA ,
    THEN
  ELSE
    FIND-PRIMITIVE
    ?DUP IF
      ( OPCODE ) C,
    ELSE CONVERT
      IF
        CODE LIT ( NUM ) ,
      ELSE ??? THEN
    THEN
  THEN ;

: MAKE-HEADER ( name -- )
  DP @
  LAST @ ,
  LAST !
  STR,
  0 ( FLAGS ) C, ;

: MAKE-WORD WORD MAKE-HEADER ;

: ;; CODE EXIT ;

: COMPILE-ENTRY
  CODE LIT $4444 , ( placeholder )
  CODE >R CODE EXIT ;

: COMPILE-HALT CODE LIT 0 , CODE HALT ;

: COMPILER-LOOP
  BEGIN
    WORD " BYE" STR= INVERT
  WHILE
    COMPILE
  REPEAT ;

: POST-CHECKS
  DEPTH 0 <> IF
     DEPTH . CR " Non empty stack." ABORT
  THEN ;

: ABORT-NAN " Not a number." ABORT ;

: FILL
  DP @ ( save current DP )
  WORD >NUMBER IF DP ! ELSE ABORT-NAN THEN
  BEGIN
    WORD DUP " .END" STR= INVERT
  WHILE
    >NUMBER IF C, ELSE ABORT-NAN THEN
  REPEAT
  DROP DP ! ( restore DP ) ;

ENTRY

\ Compile initial jump code with a placeholder address.
\ The address will be filled by ENTRY.
MAIN DP !
COMPILE-ENTRY

\ Same source code is used for stage1 and stage2 compiler.
\ Use different target address depending the stage.
STEPPER STAGE1-TARGET >= IF
  STAGE2-TARGET DP ! \ We're in stage2 compiler
ELSE
  STAGE1-TARGET DP ! \ We're in stage1 compiler
THEN

0  LAST !
10 BASE !

\ Layout user dictionary structure with essential words.

( ***************** Dictionary Structure ***************** )
( Words:                                                   )
(  16b            8b 8b                                    )
(  LINK "<name1>" 00 FLAG INSTR.1 .. INSTR.N EXIT LINK ... )
(   ^---------------------------------------------+        )

" :" MAKE-HEADER
  CODE CALL SUB MAKE-WORD
;; IMMEDIATE

" ;" MAKE-HEADER
  CODE LIT  " EXIT" >OPCODE ,
  CODE CALL SUB  C,
;; IMMEDIATE

" IMMEDIATE" MAKE-HEADER
  CODE CALL SUB IMMEDIATE
;; IMMEDIATE

" MAKE-WORD" MAKE-HEADER
  CODE CALL SUB MAKE-WORD
;;

" .FILL" MAKE-HEADER
  CODE CALL SUB FILL
;; IMMEDIATE

" ENTRY" MAKE-HEADER
  CODE LIT
  DP ,
  CODE @
  CODE LIT MAIN 1+ ,
  CODE !
;; IMMEDIATE

\ After compilation finished (BYE), dump memory to disk.
COMPILER-LOOP
COMPILE-HALT
VMPARAM-ADDR DUMP
POST-CHECKS

0 HALT

BYE
