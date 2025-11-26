#include <stdio.h>
#include <assert.h>
#include "../vm.h"

#define B0(x) ((x) & 0xFF)
#define B1(x) (((x) >> 8) & 0xFF)
#define B2(x) (((x) >> 16) & 0xFF)
#define B3(x) (((x) >> 24) & 0xFF)

#define LIT32(x) OP_LIT, B0(x), B1(x), B2(x), B3(x)

#define SIZE 255

cell_t eval(char* code) {
  return engage(code, SIZE, 0, 50, 100);
}

void test_add() {
  assert(7 == eval((char[SIZE]) {
    LIT32(2),
    LIT32(5),
    OP_ADD,
    OP_HLT
  }));

  assert(6887336 == eval((char[SIZE]) {
    LIT32(2258013),
    LIT32(4629323),
    OP_ADD,
    OP_HLT
  }));

  assert(-1371424 == eval((char[SIZE]) {
    LIT32(-429231),
    LIT32(-942193),
    OP_ADD,
    OP_HLT
  }));
}

void test_mul() {
  assert(12 == eval((char[SIZE]) {
    LIT32(3),
    LIT32(4),
    OP_MUL,
    OP_HLT
  }));

  assert(1111086179 == eval((char[SIZE]) {
    LIT32(3792103),
    LIT32(293),
    OP_MUL,
    OP_HLT
  }));

  assert(404293729 == eval((char[SIZE]) {
    LIT32(-94219),
    LIT32(-4291),
    OP_MUL,
    OP_HLT
  }));
}

void test_sub() {
  assert(6 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(4),
    OP_SUB,
    OP_HLT
  }));

  assert(100650810  == eval((char[SIZE]) {
    LIT32(313792103),
    LIT32(213141293),
    OP_SUB,
    OP_HLT
  }));

  assert(-512962 == eval((char[SIZE]) {
    LIT32(-942193),
    LIT32(-429231),
    OP_SUB,
    OP_HLT
  }));
}

void test_div() {
  assert(3 == eval((char[SIZE]) {
    LIT32(12),
    LIT32(4),
    OP_DIV,
    OP_HLT
  }));

  assert(1645 == eval((char[SIZE]) {
    LIT32(2031379210),
    LIT32(1234592),
    OP_DIV,
    OP_HLT
  }));

  assert(1525 == eval((char[SIZE]) {
    LIT32(-2022173102),
    LIT32(-1325381),
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
    LIT32(10),
    LIT32(20),
    LIT32(30),
    OP_DPT,
    OP_HLT
  }));
}

void test_drop() {
  assert(1 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    LIT32(30),
    OP_DROP,
    OP_DROP,
    OP_DPT,
    OP_HLT
  }));
}

void test_dup() {
  assert(9 == eval((char[SIZE]) {
    LIT32(3),
    OP_DUP,
    OP_MUL,
    OP_HLT
  }));
}

void test_swap() {
  assert(6 == eval((char[SIZE]) {
    LIT32(6),
    LIT32(11),
    OP_SWAP,
    OP_HLT
  }));
  assert(11 == eval((char[SIZE]) {
    LIT32(6),
    LIT32(11),
    OP_SWAP,
    OP_DROP,
    OP_HLT
  }));
  assert(2 == eval((char[SIZE]) {
    LIT32(6),
    LIT32(11),
    OP_SWAP,
    OP_DPT,
    OP_HLT
  }));
}

void test_nip() {
  assert(4 == eval((char[SIZE]) {
    LIT32(3),
    LIT32(4),
    OP_NIP,
    OP_HLT
  }));
  assert(1 == eval((char[SIZE]) {
    LIT32(3),
    LIT32(4),
    OP_NIP,
    OP_DPT,
    OP_HLT
  }));
}

void test_over() {
  assert(3 == eval((char[SIZE]) {
    LIT32(3),
    LIT32(7),
    OP_OVER,
    OP_HLT
  }));
  assert(7 == eval((char[SIZE]) {
    LIT32(3),
    LIT32(7),
    OP_OVER,
    OP_DROP,
    OP_HLT
  }));
  assert(3 == eval((char[SIZE]) {
    LIT32(3),
    LIT32(7),
    OP_OVER,
    OP_DROP,
    OP_DROP,
    OP_HLT
  }));
}

void test_rot() {
  assert(10 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    LIT32(30),
    OP_ROT,
    OP_HLT
  }));
  assert(30 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    LIT32(30),
    OP_ROT,
    OP_DROP,
    OP_HLT
  }));
  assert(20 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    LIT32(30),
    OP_ROT,
    OP_DROP,
    OP_DROP,
    OP_HLT
  }));
  assert(3 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    LIT32(30),
    OP_ROT,
    OP_DPT,
    OP_HLT
  }));
}

void test_mrot() {
  assert(20 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    LIT32(30),
    OP_MROT,
    OP_HLT
  }));
  assert(10 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    LIT32(30),
    OP_MROT,
    OP_DROP,
    OP_HLT
  }));
  assert(30 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    LIT32(30),
    OP_MROT,
    OP_DROP,
    OP_DROP,
    OP_HLT
  }));
  assert(3 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    LIT32(30),
    OP_MROT,
    OP_DPT,
    OP_HLT
  }));
}

void test_tuck() {
  assert(20 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    OP_TUCK,
    OP_HLT
  }));
  assert(10 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    OP_TUCK,
    OP_DROP,
    OP_HLT
  }));
  assert(20 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    OP_TUCK,
    OP_DROP,
    OP_DROP,
    OP_HLT
  }));
  assert(3 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(20),
    OP_TUCK,
    OP_DPT,
    OP_HLT
  }));
}

void test_inv() {
  assert(0 == eval((char[SIZE]) {
    LIT32(-1),
    OP_INV,
    OP_HLT
  }));
}

void test_jnz() {
  assert(55 == eval((char[SIZE]) {
    LIT32(10),
    LIT32(0),
    OP_OVER,
    OP_ADD,
    OP_SWAP,  
    LIT32(1), 
    OP_SUB,   
    OP_SWAP,
    OP_OVER,
    OP_JNZ,
    -12,
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
  test_inv();
  test_jnz();
  printf("All tests passed.\n");
  return 0;
}
