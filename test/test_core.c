#include <stdio.h>
#include <assert.h>
#include "../vm.h"

#define B0(x) ((x) & 0xFF)
#define B1(x) (((x) >> 8) & 0xFF)

#define LIT16(x) OP_LIT, B0(x), B1(x)

#define SIZE SHRT_MAX
#define STACK 50

cell_t eval(uint8_t* code) {
  return engage(code, 0, STACK, 100, 1024);
}

void test_add() {
  assert(7 == eval((uint8_t[SIZE]) {
    LIT16(2),
    LIT16(5),
    OP_ADD,
    OP_HLT
  }));

  assert(6887 == eval((uint8_t[SIZE]) {
    LIT16(2258),
    LIT16(4629),
    OP_ADD,
    OP_HLT
  }));

  assert(-13713 == eval((uint8_t[SIZE]) {
    LIT16(-4292),
    LIT16(-9421),
    OP_ADD,
    OP_HLT
  }));
}

void test_mul() {
  assert(12 == eval((uint8_t[SIZE]) {
    LIT16(3),
    LIT16(4),
    OP_MUL,
    OP_HLT
  }));

  assert(31717 == eval((uint8_t[SIZE]) {
    LIT16(1379),
    LIT16(23),
    OP_MUL,
    OP_HLT
  }));

  assert(27456 == eval((uint8_t[SIZE]) {
    LIT16(-64),
    LIT16(-429),
    OP_MUL,
    OP_HLT
  }));
}

void test_sub() {
  assert(6 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(4),
    OP_SUB,
    OP_HLT
  }));

  assert(1006  == eval((uint8_t[SIZE]) {
    LIT16(3137),
    LIT16(2131),
    OP_SUB,
    OP_HLT
  }));

  assert(-513 == eval((uint8_t[SIZE]) {
    LIT16(-942),
    LIT16(-429),
    OP_SUB,
    OP_HLT
  }));
}

void test_div() {
  assert(3 == eval((uint8_t[SIZE]) {
    LIT16(12),
    LIT16(4),
    OP_DIV,
    OP_HLT
  }));

  assert(16 == eval((uint8_t[SIZE]) {
    LIT16(20313),
    LIT16(1234),
    OP_DIV,
    OP_HLT
  }));

  assert(15 == eval((uint8_t[SIZE]) {
    LIT16(-20222),
    LIT16(-1325),
    OP_DIV,
    OP_HLT
  }));
}

void test_mod() {
  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(12),
    LIT16(4),
    OP_MOD,
    OP_HLT
  }));

  assert(3 == eval((uint8_t[SIZE]) {
    LIT16(12),
    LIT16(9),
    OP_MOD,
    OP_HLT
  }));

  assert(2 == eval((uint8_t[SIZE]) {
    LIT16(12),
    LIT16(5),
    OP_MOD,
    OP_HLT
  }));
}

void test_depth() {
  assert(0 == eval((uint8_t[SIZE]) {
    OP_DEPTH,
    OP_HLT
  }));

  assert(3 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_DEPTH,
    OP_HLT
  }));
}

void test_sp() {
  assert(STACK == eval((uint8_t[SIZE]) {
    OP_SP,
    OP_HLT
  }));

  assert(STACK + 3 * sizeof(cell_t) == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_SP,
    OP_HLT
  }));
}

void test_drop() {
  assert(1 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_DROP,
    OP_DROP,
    OP_DEPTH,
    OP_HLT
  }));
}

void test_dup() {
  assert(9 == eval((uint8_t[SIZE]) {
    LIT16(3),
    OP_DUP,
    OP_MUL,
    OP_HLT
  }));
}

void test_swap() {
  assert(6 == eval((uint8_t[SIZE]) {
    LIT16(6),
    LIT16(11),
    OP_SWAP,
    OP_HLT
  }));

  assert(11 == eval((uint8_t[SIZE]) {
    LIT16(6),
    LIT16(11),
    OP_SWAP,
    OP_DROP,
    OP_HLT
  }));

  assert(2 == eval((uint8_t[SIZE]) {
    LIT16(6),
    LIT16(11),
    OP_SWAP,
    OP_DEPTH,
    OP_HLT
  }));
}

void test_nip() {
  assert(4 == eval((uint8_t[SIZE]) {
    LIT16(3),
    LIT16(4),
    OP_NIP,
    OP_HLT
  }));

  assert(1 == eval((uint8_t[SIZE]) {
    LIT16(3),
    LIT16(4),
    OP_NIP,
    OP_DEPTH,
    OP_HLT
  }));
}

void test_over() {
  assert(3 == eval((uint8_t[SIZE]) {
    LIT16(3),
    LIT16(7),
    OP_OVER,
    OP_HLT
  }));

  assert(7 == eval((uint8_t[SIZE]) {
    LIT16(3),
    LIT16(7),
    OP_OVER,
    OP_DROP,
    OP_HLT
  }));

  assert(3 == eval((uint8_t[SIZE]) {
    LIT16(3),
    LIT16(7),
    OP_OVER,
    OP_DROP,
    OP_DROP,
    OP_HLT
  }));
}

void test_rot() {
  assert(10 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_ROT,
    OP_HLT
  }));

  assert(30 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_ROT,
    OP_DROP,
    OP_HLT
  }));

  assert(20 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_ROT,
    OP_DROP,
    OP_DROP,
    OP_HLT
  }));

  assert(3 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_ROT,
    OP_DEPTH,
    OP_HLT
  }));
}

void test_mrot() {
  assert(20 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_MROT,
    OP_HLT
  }));

  assert(10 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_MROT,
    OP_DROP,
    OP_HLT
  }));

  assert(30 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_MROT,
    OP_DROP,
    OP_DROP,
    OP_HLT
  }));

  assert(3 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_MROT,
    OP_DEPTH,
    OP_HLT
  }));
}

void test_tuck() {
  assert(20 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    OP_TUCK,
    OP_HLT
  }));

  assert(10 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    OP_TUCK,
    OP_DROP,
    OP_HLT
  }));

  assert(20 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    OP_TUCK,
    OP_DROP,
    OP_DROP,
    OP_HLT
  }));

  assert(3 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    OP_TUCK,
    OP_DEPTH,
    OP_HLT
  }));
}

void test_lte() {
  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(5),
    LIT16(10),
    OP_LTE,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(5),
    OP_LTE,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(5),
    LIT16(5),
    OP_LTE,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(-20),
    LIT16(-10),
    OP_LTE,
    OP_HLT
  }));
}

void test_lt() {
  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(5),
    LIT16(10),
    OP_LT,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(5),
    OP_LT,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(5),
    LIT16(5),
    OP_LT,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(-20),
    LIT16(-10),
    OP_LT,
    OP_HLT
  }));
}

void test_gt() {
  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(5),
    LIT16(2),
    OP_GT,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(2),
    LIT16(5),
    OP_GT,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(5),
    LIT16(5),
    OP_GT,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(-10),
    LIT16(-20),
    OP_GT,
    OP_HLT
  }));
}

void test_gte() {
  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(5),
    OP_GTE,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(5),
    LIT16(10),
    OP_GTE,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(5),
    LIT16(5),
    OP_GTE,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(-10),
    LIT16(-20),
    OP_GTE,
    OP_HLT
  }));
}

void test_eq() {
  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(1010),
    LIT16(1010),
    OP_EQ,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(5),
    LIT16(10),
    OP_EQ,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(-1214),
    LIT16(0),
    OP_EQ,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(-1230),
    LIT16(-1230),
    OP_EQ,
    OP_HLT
  }));
}

void test_neq() {
  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(1010),
    LIT16(1010),
    OP_NEQ,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(5),
    LIT16(10),
    OP_NEQ,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(-1214),
    LIT16(0),
    OP_NEQ,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(-1230),
    LIT16(-1230),
    OP_NEQ,
    OP_HLT
  }));
}

void test_inv() {
  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(-1),
    OP_INV,
    OP_HLT
  }));
}

void test_or() {
  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(-1),
    LIT16(-1),
    OP_OR,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(0),
    LIT16(-1),
    OP_OR,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(-1),
    LIT16(0),
    OP_OR,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(0),
    LIT16(0),
    OP_OR,
    OP_HLT
  }));
}

void test_and() {
  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(-1),
    LIT16(-1),
    OP_AND,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(0),
    LIT16(-1),
    OP_AND,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(-1),
    LIT16(0),
    OP_AND,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(0),
    LIT16(0),
    OP_AND,
    OP_HLT
  }));
}

void test_xor() {
  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(-1),
    LIT16(-1),
    OP_XOR,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(0),
    LIT16(-1),
    OP_XOR,
    OP_HLT
  }));

  assert(-1 == eval((uint8_t[SIZE]) {
    LIT16(-1),
    LIT16(0),
    OP_XOR,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(0),
    LIT16(0),
    OP_XOR,
    OP_HLT
  }));
}

void test_jnz() {
  assert(55 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(0),
    OP_OVER,
    OP_ADD,
    OP_SWAP,
    LIT16(1),
    OP_SUB,
    OP_SWAP,
    OP_OVER,
    OP_JNZ, -10,
    OP_NIP,
    OP_HLT
  }));
}

void test_jz() {
  assert(55 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(0),
    OP_OVER,
    OP_ADD,
    OP_SWAP,
    LIT16(1),
    OP_SUB,
    OP_SWAP,
    OP_OVER,
    OP_JZ,   4,
    OP_JMP, -12,
    OP_NIP,
    OP_HLT
  }));
}

void test_shl() {
  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(0),
    LIT16(1),
    OP_SHL,
    OP_HLT
  }));

  assert(2 == eval((uint8_t[SIZE]) {
    LIT16(1),
    LIT16(1),
    OP_SHL,
    OP_HLT
  }));

  assert(16 == eval((uint8_t[SIZE]) {
    LIT16(8),
    LIT16(1),
    OP_SHL,
    OP_HLT
  }));

  assert(16 == eval((uint8_t[SIZE]) {
    LIT16(4),
    LIT16(2),
    OP_SHL,
    OP_HLT
  }));
}

void test_sar() {
  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(0),
    LIT16(1),
    OP_SAR,
    OP_HLT
  }));

  assert(0 == eval((uint8_t[SIZE]) {
    LIT16(1),
    LIT16(1),
    OP_SAR,
    OP_HLT
  }));

  assert(32 == eval((uint8_t[SIZE]) {
    LIT16(256),
    LIT16(3),
    OP_SAR,
    OP_HLT
  }));

  assert(-128 == eval((uint8_t[SIZE]) {
    LIT16(-2048),
    LIT16(4),
    OP_SAR,
    OP_HLT
  }));
}

void test_fetchstore() {
  assert(42 == eval((uint8_t[SIZE]) {
    LIT16(42),
    LIT16(1000),
    OP_STO,
    LIT16(1000),
    OP_FETCH,
    OP_HLT
  }));

  assert(-23456 == eval((uint8_t[SIZE]) {
    LIT16(-23456),
    LIT16(1000),
    OP_STO,
    LIT16(1000),
    OP_FETCH,
    OP_HLT
  }));

  assert(31000 == eval((uint8_t[SIZE]) {
    LIT16(31000),
    LIT16(1000),
    OP_STO,
    LIT16(1000),
    OP_FETCH,
    OP_HLT
  }));
}

void test_comma() {
  assert(2 == eval((uint8_t[SIZE]) {
    OP_DP,
    LIT16(10),
    OP_COMA,
    OP_DP,
    OP_SWAP,
    OP_SUB,
    OP_HLT
  }));

  assert(9871 == eval((uint8_t[SIZE]) {
    OP_DP,
    LIT16(9871),
    OP_COMA,
    OP_FETCH,
    OP_HLT
  }));
}

void test_comma_byte() {
  assert(1 == eval((uint8_t[SIZE]) {
    OP_DP,
    LIT16(10),
    OP_CCOMA,
    OP_DP,
    OP_SWAP,
    OP_SUB,
    OP_HLT
  }));

  assert((9871 & 0xFF) == eval((uint8_t[SIZE]) {
    OP_DP,
    LIT16(9871),
    OP_CCOMA,
    OP_FETCH,
    OP_HLT
  }));
}

void test_todp() {
  assert(4567 == eval((uint8_t[SIZE]) {
    LIT16(4567),
    OP_TODP,
    OP_DP,
    OP_HLT
  }));
}

void test_tosp() {
  assert(8976 == eval((uint8_t[SIZE]) {
    LIT16(8976),
    OP_TOSP,
    OP_SP,
    OP_HLT
  }));
}

void test_rstack() {
  assert(1289 == eval((uint8_t[SIZE]) {
    LIT16(1289),
    LIT16(2030),
    OP_RPUSH,
    OP_HLT
  }));

  assert(1 == eval((uint8_t[SIZE]) {
    LIT16(1289),
    LIT16(2030),
    OP_RPUSH,
    OP_DEPTH,
    OP_HLT
  }));

  assert(80 == eval((uint8_t[SIZE]) {
    LIT16(10),
    LIT16(20),
    OP_RPUSH,
    OP_DUP,
    OP_MUL,
    OP_RPOP,
    OP_SUB,
    OP_HLT
  }));
}

int main() {
  test_add();
  test_mul();
  test_sub();
  test_div();
  test_mod();
  test_depth();
  test_sp();
  test_drop();
  test_dup();
  test_swap();
  test_nip();
  test_over();
  test_rot();
  test_mrot();
  test_tuck();
  test_lt();
  test_lte();
  test_gt();
  test_gte();
  test_eq();
  test_neq();
  test_inv();
  test_or();
  test_and();
  test_xor();
  test_jnz();
  test_jz();
  test_shl();
  test_sar();
  test_fetchstore();
  test_comma();
  test_comma_byte();
  test_todp();
  test_tosp();
  test_rstack();
  printf("\033[1;32mCore tests passed.\033[0m\n");
  return 0;
}
