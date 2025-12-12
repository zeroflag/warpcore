: EMIT 1 OUT ;
: C!
  DUP @ $FF00 AND
  ROT $FF AND OR
  SWAP ! ;

: C@  @ $FF AND ;
: ++  DUP @ 1+ SWAP ! ;
: ['] R> DUP C@ SWAP 1+ >R ;
:  '  R> 1+ DUP @ SWAP 2 + >R ;

: ,   $7E00 @  ! $7E00 ++ $7E00 ++ ;
: C,  $7E00 @ C! $7E00 ++ ;

: DP $7E00 ;

: EXEC >R EXIT ;
: DEPTH SP $02 - ;
: CELL 2 ;
: CELLS CELL * ;
: ALLOT DP @ + DP ! ;

: MARK    DP @ 0 , ;
: RESOLVE DP @ OVER - SWAP ! ;
: IF      ['] JZ  C, MARK ; IMMEDIATE
: ELSE    ['] JMP C, MARK SWAP RESOLVE ; IMMEDIATE
: THEN    RESOLVE ; IMMEDIATE
: BEGIN   DP @ ; IMMEDIATE
: UNTIL   ['] JZ  C, DP @ - , ; IMMEDIATE
: AGAIN   ['] JMP C, DP @ - , ; IMMEDIATE
: WHILE   ['] JZ C, MARK ; IMMEDIATE
: REPEAT  SWAP ['] JMP C, DP @ - , RESOLVE ; IMMEDIATE
: FOR     ['] >R C, DP @ ; IMMEDIATE
: NEXT
  ['] R@ C, ['] JZ  C, MARK
  ['] R> C, ['] 1-  C, ['] >R C,
  SWAP
  ['] JMP C, DP @ - ,
  RESOLVE
  ['] R> C, ['] DROP C,
; IMMEDIATE

: VARIABLE
  CREATE
  ['] LIT  C, DP @ 3 + ,
  ['] EXIT C,
  DP @ 2 + DP @ !
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

: "
  ['] LIT C, DP @ 5 + , ( addr of string )
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
: -ROT ROT ROT ;
: TUCK DUP ROT ROT ;

: BETWEEN? OVER >= -ROT <= AND ;

: CR 10 EMIT ;

: /MOD 2DUP % -ROT / ;

: . ( n -- )
    DUP 0 < IF 45 EMIT -1 * THEN
    10 /MOD ?DUP IF . THEN
    48 + EMIT ;
