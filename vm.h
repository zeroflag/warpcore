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
  OP_ADD   = 0x01,  // "+"
  OP_SUB   = 0x02,  // "-"
  OP_MUL   = 0x03,  // "*"
  OP_DIV   = 0x04,  // "/"

  OP_DUP   = 0x05,  // "DUP"
  OP_DROP  = 0x06,  // "DROP"
  OP_SWAP  = 0x07,  // "SWAP"
  OP_NIP   = 0x08,  // "NIP"
  OP_OVER  = 0x09,  // "OVER"
  OP_ROT   = 0x0A,  // "ROT"
  OP_MROT  = 0x0B,  // "-ROT"
  OP_TUCK  = 0x0C,  // "TUCK"

  OP_INV   = 0x0D,  // "INVERT"
  OP_AND   = 0x0E,  // "AND"
  OP_OR    = 0x0F,  // "OR"
  OP_XOR   = 0x10,  // "XOR"

  OP_GT    = 0x11,  // ">"
  OP_GTE   = 0x12,  // ">="
  OP_LT    = 0x13,  // "<"
  OP_LTE   = 0x14,  // "<="
  OP_EQ    = 0x15,  // "="
  OP_NEQ   = 0x16,  // "<>"

  OP_JMP   = 0x17,  // "JMP"
  OP_JZ    = 0x18,  // "JZ"
  OP_JNZ   = 0x19,  // "JNZ"
  OP_AJMP  = 0x1A,  // "AJMP"

  OP_CALL  = 0x1B,  // "CALL"
  OP_RET   = 0x1C,  // "EXIT"

  OP_DOT   = 0x1D,  // "."

  OP_NOP   = 0x1E,  // "NOP"

  OP_EMIT  = 0x1F,  // "EMIT"
  OP_LIT   = 0x20,  // "LIT"
  OP_MOD   = 0x21,  // "%"
  OP_KEY   = 0x22,  // "KEY"

  OP_SP    = 0x23,  // "SP"
  OP_TOSP  = 0x24,  // "SP!"
  OP_HLT   = 0x25,  // "HALT"

  OP_SHL   = 0x26,  // "LSHIFT"
  OP_SAR   = 0x27,  // "RSHIFT"

  OP_STO   = 0x28,  // "!"
  OP_CSTO  = 0x29,  // "C!"
  OP_FETCH = 0x2A,  // "@"
  OP_CFTCH = 0x2B,  // "C@"

  OP_DP    = 0x2C,  // "DP"
  OP_TODP  = 0x2D,  // "DP!"

  OP_COMA  = 0x2E,  // ","
  OP_CCOMA = 0x2F,  // "C,"
  OP_DEPTH = 0x30,  // "DEPTH"

  OP_RPUSH = 0x31,  // ">R"
  OP_RPOP  = 0x32,  // "R>"

  OP_EXEC  = 0x33,  // "EXEC"
  OP_DUMP  = 0x34,  // "DUMP"
} Op;

cell_t engage(uint8_t *mem,
              cell_t start_ip,
              cell_t stack,
              cell_t rstack,
              cell_t heap);

void breach(char* format, ...);

#endif // VM_H
