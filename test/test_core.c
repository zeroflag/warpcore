#include <stdio.h>
#include <assert.h>
#include "../vm.h"

#define B0(x) ((x) & 0xFF)
#define B1(x) (((x) >> 8) & 0xFF)

#define LIT16(x) OP_LIT, B0(x), B1(x)

#define SIZE SHRT_MAX

cell_t eval(char* code) {
  return engage(code, 0, 50, 100);
}

void test_add() {
  assert(7 == eval((char[SIZE]) {
    LIT16(2),
    LIT16(5),
    OP_ADD,
    OP_HLT
  }));

  assert(6887 == eval((char[SIZE]) {
    LIT16(2258),
    LIT16(4629),
    OP_ADD,
    OP_HLT
  }));

  assert(-13713 == eval((char[SIZE]) {
    LIT16(-4292),
    LIT16(-9421),
    OP_ADD,
    OP_HLT
  }));
}

void test_mul() {
  assert(12 == eval((char[SIZE]) {
    LIT16(3),
    LIT16(4),
    OP_MUL,
    OP_HLT
  }));

  assert(31717 == eval((char[SIZE]) {
    LIT16(1379),
    LIT16(23),
    OP_MUL,
    OP_HLT
  }));

  assert(27456 == eval((char[SIZE]) {
    LIT16(-64),
    LIT16(-429),
    OP_MUL,
    OP_HLT
  }));
}

void test_sub() {
  assert(6 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(4),
    OP_SUB,
    OP_HLT
  }));

  assert(1006  == eval((char[SIZE]) {
    LIT16(3137),
    LIT16(2131),
    OP_SUB,
    OP_HLT
  }));

  assert(-513 == eval((char[SIZE]) {
    LIT16(-942),
    LIT16(-429),
    OP_SUB,
    OP_HLT
  }));
}

void test_div() {
  assert(3 == eval((char[SIZE]) {
    LIT16(12),
    LIT16(4),
    OP_DIV,
    OP_HLT
  }));

  assert(16 == eval((char[SIZE]) {
    LIT16(20313),
    LIT16(1234),
    OP_DIV,
    OP_HLT
  }));

  assert(15 == eval((char[SIZE]) {
    LIT16(-20222),
    LIT16(-1325),
    OP_DIV,
    OP_HLT
  }));
}

void test_depth() {
  assert(0 == eval((char[SIZE]) {
    OP_DPT,
    OP_HLT
  }));

  assert(3 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_DPT,
    OP_HLT
  }));
}

void test_drop() {
  assert(1 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_DROP,
    OP_DROP,
    OP_DPT,
    OP_HLT
  }));
}

void test_dup() {
  assert(9 == eval((char[SIZE]) {
    LIT16(3),
    OP_DUP,
    OP_MUL,
    OP_HLT
  }));
}

void test_swap() {
  assert(6 == eval((char[SIZE]) {
    LIT16(6),
    LIT16(11),
    OP_SWAP,
    OP_HLT
  }));
  assert(11 == eval((char[SIZE]) {
    LIT16(6),
    LIT16(11),
    OP_SWAP,
    OP_DROP,
    OP_HLT
  }));
  assert(2 == eval((char[SIZE]) {
    LIT16(6),
    LIT16(11),
    OP_SWAP,
    OP_DPT,
    OP_HLT
  }));
}

void test_nip() {
  assert(4 == eval((char[SIZE]) {
    LIT16(3),
    LIT16(4),
    OP_NIP,
    OP_HLT
  }));
  assert(1 == eval((char[SIZE]) {
    LIT16(3),
    LIT16(4),
    OP_NIP,
    OP_DPT,
    OP_HLT
  }));
}

void test_over() {
  assert(3 == eval((char[SIZE]) {
    LIT16(3),
    LIT16(7),
    OP_OVER,
    OP_HLT
  }));
  assert(7 == eval((char[SIZE]) {
    LIT16(3),
    LIT16(7),
    OP_OVER,
    OP_DROP,
    OP_HLT
  }));
  assert(3 == eval((char[SIZE]) {
    LIT16(3),
    LIT16(7),
    OP_OVER,
    OP_DROP,
    OP_DROP,
    OP_HLT
  }));
}

void test_rot() {
  assert(10 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_ROT,
    OP_HLT
  }));
  assert(30 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_ROT,
    OP_DROP,
    OP_HLT
  }));
  assert(20 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_ROT,
    OP_DROP,
    OP_DROP,
    OP_HLT
  }));
  assert(3 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_ROT,
    OP_DPT,
    OP_HLT
  }));
}

void test_mrot() {
  assert(20 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_MROT,
    OP_HLT
  }));
  assert(10 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_MROT,
    OP_DROP,
    OP_HLT
  }));
  assert(30 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_MROT,
    OP_DROP,
    OP_DROP,
    OP_HLT
  }));
  assert(3 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    LIT16(30),
    OP_MROT,
    OP_DPT,
    OP_HLT
  }));
}

void test_tuck() {
  assert(20 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    OP_TUCK,
    OP_HLT
  }));
  assert(10 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    OP_TUCK,
    OP_DROP,
    OP_HLT
  }));
  assert(20 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    OP_TUCK,
    OP_DROP,
    OP_DROP,
    OP_HLT
  }));
  assert(3 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(20),
    OP_TUCK,
    OP_DPT,
    OP_HLT
  }));
}

void test_lt() {
  assert(-1 == eval((char[SIZE]) {
    LIT16(5),
    LIT16(10),
    OP_LT,
    OP_HLT
  }));
  assert(0 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(5),
    OP_LT,
    OP_HLT
  }));
  assert(0 == eval((char[SIZE]) {
    LIT16(5),
    LIT16(5),
    OP_LT,
    OP_HLT
  }));
  assert(-1 == eval((char[SIZE]) {
    LIT16(-20),
    LIT16(-10),
    OP_LT,
    OP_HLT
  }));
}

void test_gt() {
  assert(-1 == eval((char[SIZE]) {
    LIT16(5),
    LIT16(2),
    OP_GT,
    OP_HLT
  }));
  assert(0 == eval((char[SIZE]) {
    LIT16(2),
    LIT16(5),
    OP_GT,
    OP_HLT
  }));
  assert(0 == eval((char[SIZE]) {
    LIT16(5),
    LIT16(5),
    OP_GT,
    OP_HLT
  }));
  assert(-1 == eval((char[SIZE]) {
    LIT16(-10),
    LIT16(-20),
    OP_GT,
    OP_HLT
  }));
}

void test_inv() {
  assert(0 == eval((char[SIZE]) {
    LIT16(-1),
    OP_INV,
    OP_HLT
  }));
}

void test_or() {
  assert(-1 == eval((char[SIZE]) {
    LIT16(-1),
    LIT16(-1),
    OP_OR,
    OP_HLT
  }));
  assert(-1 == eval((char[SIZE]) {
    LIT16(0),
    LIT16(-1),
    OP_OR,
    OP_HLT
  }));
  assert(-1 == eval((char[SIZE]) {
    LIT16(-1),
    LIT16(0),
    OP_OR,
    OP_HLT
  }));
  assert(0 == eval((char[SIZE]) {
    LIT16(0),
    LIT16(0),
    OP_OR,
    OP_HLT
  }));
}

void test_and() {
  assert(-1 == eval((char[SIZE]) {
    LIT16(-1),
    LIT16(-1),
    OP_AND,
    OP_HLT
  }));
  assert(0 == eval((char[SIZE]) {
    LIT16(0),
    LIT16(-1),
    OP_AND,
    OP_HLT
  }));
  assert(0 == eval((char[SIZE]) {
    LIT16(-1),
    LIT16(0),
    OP_AND,
    OP_HLT
  }));
  assert(0 == eval((char[SIZE]) {
    LIT16(0),
    LIT16(0),
    OP_AND,
    OP_HLT
  }));
}

void test_jnz() {
  assert(55 == eval((char[SIZE]) {
    LIT16(10),
    LIT16(0),
    OP_OVER,
    OP_ADD,
    OP_SWAP,  
    LIT16(1), 
    OP_SUB,   
    OP_SWAP,
    OP_OVER,
    OP_JNZ,
    -10,
    OP_NIP,
    OP_HLT
  }));
}

int main() {
  test_add();
  test_mul();
  test_sub();
  test_div();
  test_depth();
  test_drop();
  test_dup();
  test_swap();
  test_nip();
  test_over();
  test_rot();
  test_mrot();
  test_tuck();
  test_lt();
  test_gt();
  test_inv();
  test_or();
  test_and();
  test_jnz();
  printf("All tests passed.\n");
  return 0;
}

