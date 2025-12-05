: SQR DUP * ;

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

: MIN 2DUP < IF DROP ELSE NIP  THEN ;
: MAX 2DUP < IF NIP  ELSE DROP THEN ;

VARIABLE V1
VARIABLE V2

2  CONSTANT TWO
10 CONSTANT TEN

ENTRY

TWO SQR 3 SQR MIN . CR
3   SQR 2 SQR MIN . CR

2 SQR 3   SQR MAX . CR
3 SQR TWO SQR MAX . CR

TEN SUMN . CR

4243 V1 !
V1 @ . CR
V1 @ 1 + . CR

7 FACTORIAL . CR

s" Test String"   PRINT CR
s" Test String"   PRINT CR
s" Test String 2" PRINT CR

1 DAY PRINT CR
2 DAY PRINT CR
3 DAY PRINT CR
5 DAY PRINT CR
8 DAY PRINT . CR

DEPTH . CR

BYE
