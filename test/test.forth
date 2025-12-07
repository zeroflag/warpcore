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

: LENGTH
  0
  BEGIN
    OVER C@ 0 <>
  WHILE
    1 + SWAP
    1 + SWAP
  REPEAT
  NIP ;

VARIABLE V1
VARIABLE V2

( 10 CONSTANT TEN )
: TEN 10 ; ( TODO recheck constant impl. )

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

( Test multi
       line
       comment )

s" Test String"   PRINT CR
s" Test String"   PRINT CR
s" Test String 2" PRINT CR

\ test single line comment

1 DAY PRINT CR
2 DAY PRINT CR
3 DAY PRINT CR
5 DAY PRINT CR
8 DAY PRINT . CR

s" Hexa test" PRINT CR
$FF . CR
$10 . CR
$5A . CR

s" Hello World !" LENGTH . CR

9 FOR I . NEXT CR

s" depth" PRINT CR
DEPTH . CR

0 HALT

BYE
