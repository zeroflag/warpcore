#ifndef VH_H
#define VM_H

#include <stdint.h>
#include <limits.h>

#define PUSH(val)   ( *sp = (val), sp++ )
#define POP         ( *(--sp) )

#define RPUSH(val)  ( *rp++ = (val) )
#define RPOP        ( *(--rp) )

#define LO(val)    ((val) & 0xFF)
#define HI(val)    (((val) >> 8) & 0xFF)

#define BINARY(operator) ( *(sp-2) operator *(sp-1), sp-- )
#define UNARY(operator, operand) ( *(sp-1) operator (operand) )
#define NULLARY(operator) ( *(sp-1) = operator *(sp-1) )
#define COMPARE(operator) ( *(sp-2) = *(sp-2) operator *(sp-1) ? TRUE : FALSE, sp-- )
#define JUMP_IF(cond) (ip += (cond) ? fetch_cell(ip) : (int)sizeof(cell_t))

typedef int16_t cell_t;
typedef uint8_t opcode_t;

extern const cell_t MEM_SIZE;

typedef enum {
  OP_ADD   = 0x01,  // "+"
  OP_SUB   = 0x02,  // "-"
  OP_MUL   = 0x03,  // "*"
  OP_DIV   = 0x04,  // "/"
  OP_INC   = 0x05,  // "1+"
  OP_DEC   = 0x06,  // "1-"
  OP_DUP   = 0x07,  // "DUP"
  OP_DROP  = 0x08,  // "DROP"
  OP_SWAP  = 0x09,  // "SWAP"
  OP_NIP   = 0x0A,  // "NIP"
  OP_OVER  = 0x0B,  // "OVER"
  OP_ROT   = 0x0C,  // "ROT"
  OP_MROT  = 0x0D,  // "-ROT"
  OP_TUCK  = 0x0E,  // "TUCK"
  OP_INV   = 0x0F,  // "INVERT"
  OP_AND   = 0x10,  // "AND"
  OP_OR    = 0x11,  // "OR"
  OP_XOR   = 0x12,  // "XOR"
  OP_GT    = 0x13,  // ">"
  OP_GTE   = 0x14,  // ">="
  OP_LT    = 0x15,  // "<"
  OP_LTE   = 0x16,  // "<="
  OP_EQ    = 0x17,  // "="
  OP_NEQ   = 0x18,  // "<>"
  OP_JMP   = 0x19,  // "JMP"
  OP_JZ    = 0x1A,  // "JZ"
  OP_JNZ   = 0x1B,  // "JNZ"
  OP_AJMP  = 0x1C,  // "AJMP"
  OP_CALL  = 0x1D,  // "CALL"
  OP_RET   = 0x1E,  // "EXIT"
  OP_DOT   = 0x1F,  // "."
  OP_NOP   = 0x20,  // "NOP"
  OP_EMIT  = 0x21,  // "EMIT"
  OP_LIT   = 0x22,  // "LIT"
  OP_MOD   = 0x23,  // "%"
  OP_KEY   = 0x24,  // "KEY"
  OP_SP    = 0x25,  // "SP"
  OP_TOSP  = 0x26,  // "SP!"
  OP_HLT   = 0x27,  // "HALT"
  OP_SHL   = 0x28,  // "LSHIFT"
  OP_SAR   = 0x29,  // "RSHIFT"
  OP_STO   = 0x2A,  // "!"
  OP_CSTO  = 0x2B,  // "C!"
  OP_FETCH = 0x2C,  // "@"
  OP_CFTCH = 0x2D,  // "C@"
  OP_DP    = 0x2E,  // "DP"
  OP_TODP  = 0x2F,  // "DP!"
  OP_DEPTH = 0x30,  // "DEPTH"
  OP_RPUSH = 0x31,  // ">R"
  OP_RPOP  = 0x32,  // "R>"
  OP_RTOP  = 0x33,  // "R@"
  OP_EXEC  = 0x34,  // "EXEC"
  OP_DUMP  = 0x35,  // "DUMP"
  OP_ABORT = 0x36,  // "ABORT"
  OP_BTICK = 0x37,  // "[']"
  OP_CTICK = 0x38,  // "'"
} Op;

cell_t engage(uint8_t *mem,
              cell_t start_ip,
              cell_t stack,
              cell_t rstack,
              cell_t heap);

void breach(char* format, ...);

#endif // VM_H
