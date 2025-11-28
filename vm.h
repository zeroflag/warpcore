#ifndef VH_H
#define VM_H

#include <stdint.h>
#include <limits.h>

#define PUSH(val)   ( *sp++  = (val) )
#define POP         ( *(--sp) )

#define RPUSH(val)  ( *rp++ = (val) )
#define RPOP        ( *(--rp) )

#define LO(val)    ((val) & 0xFF)
#define HI(val)    (((val) >> 8) & 0xFF)

typedef int16_t cell_t;
typedef uint8_t opcode_t;

extern const cell_t MEM_SIZE;

typedef enum {
  OP_ADD   = 0x01,
  OP_SUB   = 0x02,
  OP_MUL   = 0x03,
  OP_DIV   = 0x04,

  OP_DUP   = 0x05,
  OP_DROP  = 0x06,
  OP_SWAP  = 0x07,
  OP_NIP   = 0x08,
  OP_OVER  = 0x09,
  OP_ROT   = 0x0A,
  OP_MROT  = 0x0B,
  OP_TUCK  = 0x0C,

  OP_INV   = 0x0D,
  OP_AND   = 0x0E,
  OP_OR    = 0x0F,
  OP_XOR   = 0x10,

  OP_GT    = 0x11,
  OP_GTE   = 0x12,
  OP_LT    = 0x13,
  OP_LTE   = 0x14,
  OP_EQ    = 0x15,
  OP_NEQ   = 0x16,

  OP_JMP   = 0x17,
  OP_JZ    = 0x18,
  OP_JNZ   = 0x19,

  OP_CALL  = 0x1A,
  OP_RET   = 0x1B,

  OP_DOT   = 0x1C,

  OP_NOP   = 0x1D,

  OP_EMIT  = 0x1E,
  OP_LIT   = 0x1F,
  OP_MOD   = 0x20,
  OP_KEY   = 0x21,

  OP_SP    = 0x22,
  OP_TOSP  = 0x23,
  OP_HLT   = 0x24,

  OP_SHL   = 0x25,
  OP_SAR   = 0x26,

  OP_STO   = 0x27,
  OP_FETCH = 0x28,

  OP_DP    = 0x29,
  OP_TODP  = 0x2A,

  OP_COMA  = 0x2B,
  OP_CCOMA = 0x2C,
  OP_DEPTH = 0x2D,

  OP_RPUSH = 0x2E,
  OP_RPOP  = 0x2F,
} Op;

cell_t engage(uint8_t *mem,
              cell_t start_ip,
              cell_t stack,
              cell_t rstack,
              cell_t heap);

void breach(char* format, ...);

#endif // VM_H
