: ,   DP   ! DP 2 + DP! ;
: C,  DP  C! DP 1 + DP! ;
: EXEC >R EXIT ;

: MARK    DP 0 , ;
: RESOLVE DP OVER - SWAP ! ;
: IF      ['] JZ  C, MARK ; IMMEDIATE
: ELSE    ['] JMP C, MARK SWAP RESOLVE ; IMMEDIATE
: THEN    RESOLVE ; IMMEDIATE
: BEGIN   DP ; IMMEDIATE
: UNTIL   ['] JZ  C, DP - , ; IMMEDIATE
: AGAIN   ['] JMP C, DP - , ; IMMEDIATE
: WHILE   ['] JZ C, MARK ; IMMEDIATE
: REPEAT  SWAP ['] JMP C, DP - , RESOLVE ; IMMEDIATE
: FOR     ['] >R C, DP ; IMMEDIATE
: NEXT
  ['] R@ C, ['] JZ  C, MARK
  ['] R> C, ['] LIT C, 1 , ['] - C, ['] >R C,
  SWAP
  ['] JMP C, DP - ,
  RESOLVE
  ['] R> C, ['] DROP C,
; IMMEDIATE
  
: VARIABLE
  CREATE
  ['] LIT  C, DP 3 + ,
  ['] EXIT C,
  DP 2 + DP!
; IMMEDIATE 

: CONSTANT
  DP 2 - @ >R
  DP 3 - DP!
  CREATE
  ['] LIT  C, R> ,
  ['] EXIT C,
; IMMEDIATE 

: ( BEGIN KEY 41 = UNTIL ; IMMEDIATE
: \ BEGIN KEY DUP 10 = SWAP 13 = OR UNTIL ; IMMEDIATE

: CASE 0 ; IMMEDIATE
: OF
  ['] OVER C, ['] = C,
  ['] JZ   C, MARK
  ['] DROP C,
; IMMEDIATE

: ENDOF
    SWAP 1 + SWAP
    ['] JMP C, MARK SWAP
    RESOLVE
    SWAP
; IMMEDIATE

: ENDCASE
  BEGIN
    DUP 0 >
  WHILE
    1 - SWAP RESOLVE
  REPEAT
  DROP
; IMMEDIATE

: NON-ZERO? C@ 0 <> ;

: PRINT
  BEGIN
    DUP NON-ZERO?
  WHILE
    DUP C@ EMIT
    1 +
  REPEAT
  DROP ;

: s"
  ['] LIT C, DP 5 + , ( addr of string )
  ['] JMP C, MARK
  BEGIN
    KEY DUP 34 <>
  WHILE
    C,
  REPEAT
  DROP
  0 C,
  RESOLVE
; IMMEDIATE

: ?DUP  DUP 0 <> IF DUP THEN ;
: 2DUP OVER OVER ;
: 2DROP DROP DROP ;
: 3DROP DROP DROP DROP ;

: CR 10 EMIT ;

: /MOD 2DUP % -ROT / ;

: . ( n -- )
    DUP 0 < IF 45 EMIT -1 * THEN
    10 /MOD ?DUP IF . THEN
    48 + EMIT ;
