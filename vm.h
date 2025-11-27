#ifndef VH_H
#define VM_H

#include <stdint.h>
#include <limits.h>

#define PUSH(val)   ( *sp++  = (val) )
#define POP         ( *(--sp) )

#define RPUSH(val)  ( *rp++ = (val) )
#define RPOP        ( *(--rp) )

typedef int16_t cell_t;
typedef uint8_t opcode_t;

extern const cell_t MEM_SIZE;

typedef enum {
  OP_ADD  = 0x01,
  OP_SUB  = 0x02,
  OP_MUL  = 0x03,
  OP_DIV  = 0x04,

  OP_DUP  = 0x05,
  OP_DROP = 0x06,
  OP_SWAP = 0x07,
  OP_NIP  = 0x08,
  OP_OVER = 0x09,
  OP_ROT  = 0x0A,
  OP_MROT = 0x0B,
  OP_TUCK = 0x0C,

  OP_INV  = 0x0D,
  OP_AND  = 0x0E,
  OP_OR   = 0x0F,
  OP_XOR  = 0x10,

  OP_GT   = 0x11,
  OP_GTE  = 0x12,
  OP_LT   = 0x13,
  OP_LTE  = 0x14,
  OP_EQ   = 0x15,
  OP_NEQ  = 0x16,
  
  OP_JMP  = 0x17,
  OP_JZ   = 0x18,
  OP_JNZ  = 0x19,

  OP_CALL = 0x1A,
  OP_RET  = 0x1B,

  OP_PRN  = 0x1C,
  OP_CR   = 0x1D,
  OP_LIT  = 0x1E,
  OP_MOD  = 0x1F,

  OP_DPT  = 0x20,
  OP_HLT  = 0x21,
} Op;

cell_t engage(char *mem,
              cell_t start_ip,
              cell_t stack,
              cell_t rstack);

void breach(char* format, ...);

#endif // VM_H
