: SQR DUP * ;
: CR 10 EMIT ;

: 2DUP OVER OVER ;
: 2DROP DROP DROP ;

: MIN 2DUP < IF DROP ELSE NIP  THEN ;
: MAX 2DUP < IF NIP  ELSE DROP THEN ;

CONSTANT TEN 10

VARIABLE V1

: SUMN
  0
  BEGIN
    OVER +
    SWAP 1 - SWAP
    OVER 0 =
  UNTIL
  NIP ;

: FACTORIAL
  1 2 ROT
  BEGIN
      2DUP <=
  WHILE
      -ROT TUCK
      * SWAP
      1 + ROT
  REPEAT
  2DROP ;

: NON-ZERO? C@ 0 <> ;

: TYPE
  BEGIN
    DUP NON-ZERO?
  WHILE
    DUP C@ EMIT
    1 +
  REPEAT
  DROP ;

: DAY ( n -- s )
  CASE
    1 OF s" Monday"  ENDOF
    2 OF s" Tuesday" ENDOF
    3 OF s" Wednesday" ENDOF
    4 OF s" Thursday" ENDOF
    5 OF s" Friday" ENDOF
    6 OF s" Saturday" ENDOF
    7 OF s" Sunday" ENDOF
    ( number left on stack )
    s" Unknown day: "
  ENDCASE ;

ENTRY

2 SQR 3 SQR MIN . CR
3 SQR 2 SQR MIN . CR

2 SQR 3 SQR MAX . CR
3 SQR 2 SQR MAX . CR

TEN SUMN . CR

4243 V1 !
V1 @ . CR
V1 @ 1 + . CR

7 FACTORIAL . CR

s" Test String"   TYPE CR
s" Test String"   TYPE CR
s" Test String 2" TYPE CR

1 DAY TYPE CR
2 DAY TYPE CR
3 DAY TYPE CR
5 DAY TYPE CR
8 DAY TYPE . CR

DEPTH . CR

0 HALT
