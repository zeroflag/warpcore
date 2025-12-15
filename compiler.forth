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

: TRUE      -1 ;
: FALSE      0 ;

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

: STORE POS @ C! POS ++ ;
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

: >OPCODE ( s -- opcode / 0 )
  DUP " +"        STR= IF DROP $01 EXIT THEN
  DUP " -"        STR= IF DROP $02 EXIT THEN
  DUP " *"        STR= IF DROP $03 EXIT THEN
  DUP " /"        STR= IF DROP $04 EXIT THEN
  DUP " 1+"       STR= IF DROP $05 EXIT THEN
  DUP " 1-"       STR= IF DROP $06 EXIT THEN
  DUP " DUP"      STR= IF DROP $07 EXIT THEN
  DUP " DROP"     STR= IF DROP $08 EXIT THEN
  DUP " SWAP"     STR= IF DROP $09 EXIT THEN
  DUP " NIP"      STR= IF DROP $0A EXIT THEN
  DUP " OVER"     STR= IF DROP $0B EXIT THEN
  DUP " ROT"      STR= IF DROP $0C EXIT THEN
  DUP " INVERT"   STR= IF DROP $0D EXIT THEN
  DUP " AND"      STR= IF DROP $0E EXIT THEN
  DUP " OR"       STR= IF DROP $0F EXIT THEN
  DUP " XOR"      STR= IF DROP $10 EXIT THEN
  DUP " >"        STR= IF DROP $11 EXIT THEN
  DUP " >="       STR= IF DROP $12 EXIT THEN
  DUP " <"        STR= IF DROP $13 EXIT THEN
  DUP " <="       STR= IF DROP $14 EXIT THEN
  DUP " ="        STR= IF DROP $15 EXIT THEN
  DUP " <>"       STR= IF DROP $16 EXIT THEN
  DUP " JMP"      STR= IF DROP $17 EXIT THEN
  DUP " JZ"       STR= IF DROP $18 EXIT THEN
  DUP " CALL"     STR= IF DROP $19 EXIT THEN
  DUP " IN"       STR= IF DROP $1A EXIT THEN
  DUP " OUT"      STR= IF DROP $1B EXIT THEN
  DUP " LIT"      STR= IF DROP $1C EXIT THEN
  DUP " %"        STR= IF DROP $1D EXIT THEN
  DUP " EXIT"     STR= IF DROP $1E EXIT THEN
  DUP " SP"       STR= IF DROP $1F EXIT THEN
  DUP " HALT"     STR= IF DROP $20 EXIT THEN
  DUP " LSHIFT"   STR= IF DROP $21 EXIT THEN
  DUP " RSHIFT"   STR= IF DROP $22 EXIT THEN
  DUP " !"        STR= IF DROP $23 EXIT THEN
  DUP " @"        STR= IF DROP $24 EXIT THEN
  DUP " >R"       STR= IF DROP $25 EXIT THEN
  DUP " R>"       STR= IF DROP $26 EXIT THEN
  DUP " R@"       STR= IF DROP $27 EXIT THEN
  DUP " I"        STR= IF DROP $27 EXIT THEN \ Alias to R@
  DUP " DUMP"     STR= IF DROP $28 EXIT THEN
  DUP " ABORT"    STR= IF DROP $29 EXIT THEN
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
  DP @
  LAST @ ,
  LAST !
  STR,
  0 ( FLAGS ) C, ;

: MAKE-WORD WORD MAKE-HEADER ;

: ;; ['] EXIT C, ;

: COMPILE-ENTRY
  ['] LIT C, $4444 , ( placeholder )
  ['] >R C, ['] EXIT C, ;

: COMPILE-HALT ['] LIT C, 0 , ['] HALT C, ;

: COMPILER-LOOP
  BEGIN
    WORD " BYE" STR= INVERT
  WHILE
    COMPILE
  REPEAT ;

: POST-CHECKS
  DEPTH 0 <> IF
    " Non Empty stack: " PRINT DEPTH . CR
    ABORT
  THEN ;

: FILL
  DP @ ( save current DP )
  WORD >NUMBER IF DP ! ELSE ABORT THEN
  BEGIN
    WORD DUP " .END" STR= INVERT
  WHILE
    >NUMBER IF C, ELSE ABORT THEN
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
  ['] CALL C, ' MAKE-WORD  ,
;; IMMEDIATE

" ;" MAKE-HEADER
  ['] LIT  C, ['] EXIT ,
  ['] CALL C,  '  C,   ,  \ C, is not a primitive
;; IMMEDIATE

" IMMEDIATE" MAKE-HEADER
  ['] CALL C, ' IMMEDIATE ,
;; IMMEDIATE

" MAKE-WORD" MAKE-HEADER
  ['] CALL C, ' MAKE-WORD ,
;;

" .FILL" MAKE-HEADER
  ['] CALL C, ' FILL ,
;; IMMEDIATE

" ENTRY" MAKE-HEADER
  ['] LIT C,
  DP ,
  ['] @  C,
  ['] LIT C, MAIN 1+ ,
  ['] !   C,
;; IMMEDIATE

\ After compilation finished (BYE), dump memory to disk.
COMPILER-LOOP
COMPILE-HALT
VMPARAM-ADDR DUMP
POST-CHECKS

0 HALT

BYE
