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

#define SET_IP(val) (ip = (opcode_t *) (mem + (val)))
#define SET_SP(val) (sp = (cell_t *) (mem + (val)))
#define SET_RP(val) (rp = (cell_t *) (mem + (val)))

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
  OP_TUCK  = 0x0D,  // "TUCK"
  OP_INV   = 0x0E,  // "INVERT"
  OP_AND   = 0x0F,  // "AND"
  OP_OR    = 0x10,  // "OR"
  OP_XOR   = 0x11,  // "XOR"
  OP_GT    = 0x12,  // ">"
  OP_GTE   = 0x13,  // ">="
  OP_LT    = 0x14,  // "<"
  OP_LTE   = 0x15,  // "<="
  OP_EQ    = 0x16,  // "="
  OP_NEQ   = 0x17,  // "<>"
  OP_JMP   = 0x18,  // "JMP"
  OP_JZ    = 0x19,  // "JZ"
  OP_CALL  = 0x1A,  // "CALL"
  OP_RET   = 0x1B,  // "EXIT"
  OP_EMIT  = 0x1C,  // "EMIT"
  OP_LIT   = 0x1D,  // "LIT"
  OP_MOD   = 0x1E,  // "%"
  OP_KEY   = 0x1F,  // "KEY"
  OP_SP    = 0x20,  // "SP"
  OP_HLT   = 0x21,  // "HALT"
  OP_SHL   = 0x22,  // "LSHIFT"
  OP_SAR   = 0x23,  // "RSHIFT"
  OP_STOR = 0x24,  // "!"
  OP_LOAD  = 0x25,  // "@"
  OP_RPUSH = 0x26,  // ">R"
  OP_RPOP  = 0x27,  // "R>"
  OP_RTOP  = 0x28,  // "R@"
  OP_DUMP  = 0x29,  // "DUMP"
  OP_ABORT = 0x2A,  // "ABORT"
} Op;

cell_t engage(uint8_t *mem,
              cell_t start_ip,
              cell_t stack,
              cell_t rstack);

void breach(char* format, ...);

#endif // VM_H
