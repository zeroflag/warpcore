#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "vm.h"

#define DEBUG 0

const cell_t MEM_SIZE = SHRT_MAX;

const cell_t TRUE  = -1;
const cell_t FALSE =  0;

void breach(char* format, ...) {
  va_list args;
  fprintf(stderr, "[ERROR] ");
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(1);
}

inline cell_t fetch_cell(const opcode_t* addr) {
  return (cell_t) ((uint16_t) *addr | ((uint16_t) *(addr+1) << 8));
}

cell_t engage(uint8_t *mem,
              cell_t start_ip,
              cell_t stack,
              cell_t rstack,
              cell_t heap)
{
  register cell_t*   sp = (cell_t*)   (mem + stack);
  register cell_t*   rp = (cell_t*)   (mem + rstack);
  register opcode_t* ip = (opcode_t*) (mem + start_ip);

  uint8_t* dp = (uint8_t*) (mem + heap);

  while (((uint8_t*)ip - mem) < MEM_SIZE) {
    opcode_t code = *ip++;
    #if DEBUG
      printf("[0x%X] OPCODE: 0x%X\n", (cell_t)(ip - mem), code);
    #endif
    switch (code) {
      case OP_ADD: {
        *(sp-2) += *(sp-1);
        sp--;
        break;
      }
      case OP_SUB: {
        *(sp-2) -= *(sp-1);
        sp--;
        break;
      }
      case OP_MUL: {
        *(sp-2) *= *(sp-1);
        sp--;
        break;
      }
      case OP_DIV: {
        *(sp-2) /= *(sp-1);
        sp--;
        break;
      }
      case OP_DUP: { // ( a -- a a )
        *sp = *(sp-1);
        sp++;
        break;
      }
      case OP_DROP: { // ( a --  )
        sp--;
        break;
      }
      case OP_SWAP: { // ( a b -- b a )
        cell_t tmp = *(sp-1);
        *(sp-1) = *(sp-2);
        *(sp-2) = tmp;
        break;
      }
      case OP_NIP: { // ( a b -- b )
        *(sp-2) = *(sp-1);
        sp--;
        break;
      }
      case OP_OVER: { // ( a b -- a b a )
        *sp = *(sp-2);
        sp++;
        break;
      }
      case OP_ROT: { // ( a b c -- b c a )
        cell_t tmp = *(sp-3);
        *(sp-3) = *(sp-2);
        *(sp-2) = *(sp-1);
        *(sp-1) = tmp;
        break;
      }
      case OP_MROT: { // ( a b c -- c a b )
        cell_t tmp = *(sp-1);
        *(sp-1) = *(sp-2);
        *(sp-2) = *(sp-3);
        *(sp-3) = tmp;
        break;
      }
      case OP_TUCK: { // ( a b -- b a b )
        cell_t tmp = *(sp-2);
        *(sp-2) = *(sp-1);
        *(sp-1) = tmp;
        *sp = *(sp-2);
        sp++;
        break;
      }
      case OP_LIT: {
        PUSH(*(cell_t *) ip);
        ip += sizeof(cell_t);
        break;
      }
      case OP_EQ: {
        *(sp-2) = *(sp-2) == *(sp-1) ? TRUE : FALSE;
        sp--;
        break;
      }
      case OP_NEQ: {
        *(sp-2) = *(sp-2) != *(sp-1) ? TRUE : FALSE;
        sp--;
        break;
      }
      case OP_LT: {
        *(sp-2) = *(sp-2) < *(sp-1) ? TRUE : FALSE;
        sp--;
        break;
      }
      case OP_LTE: {
        *(sp-2) = *(sp-2) <= *(sp-1) ? TRUE : FALSE;
        sp--;
        break;
      }
      case OP_GT: {
        *(sp-2) = *(sp-2) > *(sp-1) ? TRUE : FALSE;
        sp--;
        break;
      }
      case OP_GTE: {
        *(sp-2) = *(sp-2) >= *(sp-1) ? TRUE : FALSE;
        sp--;
        break;
      }
      case OP_INV: {
        *(sp-1) = ~ *(sp-1);
        break;
      }
      case OP_AND: {
        *(sp-2) &= *(sp-1);
        sp--;
        break;
      }
      case OP_OR: {
        *(sp-2) |= *(sp-1);
        sp--;
        break;
      }
      case OP_XOR: {
        *(sp-2) ^= *(sp-1);
        sp--;
        break;
      }
      case OP_MOD: {
        *(sp-2) %= *(sp-1);
        sp--;
        break;
      }
      case OP_STO: {
        cell_t addr = POP;
        cell_t val  = POP;
        mem[addr] = LO(val);
        mem[addr +1] = HI(val);
        break;
      }
      case OP_CSTO: {
        cell_t addr = POP;
        cell_t val  = POP;
        mem[addr] = LO(val);
        break;
      }
      case OP_FETCH: {
        cell_t addr = POP;
        cell_t val = (uint16_t)mem[addr] | ((uint16_t)mem[addr +1] << 8);
        PUSH(val);
        break;
      }
      case OP_CFTCH: {
        cell_t addr = POP;
        cell_t val = mem[addr];
        PUSH(val);
        break;
      }
      case OP_JZ: {
        ip += (POP == 0) ? fetch_cell(ip) : (int)sizeof(cell_t);
        break; 
      }
      case OP_JNZ: {
        ip += (POP != 0) ? fetch_cell(ip) : (int)sizeof(cell_t);
        break; 
      }
      case OP_JMP: {
        ip += fetch_cell(ip);
        break;
      }
      case OP_AJMP: {
        ip = (opcode_t *) (mem + fetch_cell(ip));
        break;
      }
      case OP_CALL: {
        cell_t addr = (cell_t) ((uint8_t*)ip - mem + sizeof(uint16_t));
        RPUSH(addr);
        ip = (opcode_t *) (mem + fetch_cell(ip));
        break;
      }
      case OP_RET: {
        cell_t addr = RPOP;
        ip = (opcode_t *) (mem + addr);
        break;
      }
      case OP_EXEC: {
        cell_t addr = (cell_t) ((uint8_t*)ip - mem);
        RPUSH(addr);
        ip = (opcode_t *) (mem + POP);
        break;
      }
      case OP_KEY: {
        PUSH(getchar());
        break;
      }
      case OP_DOT: {
        printf("%d", POP);
        break;
      }
      case OP_EMIT: {
        printf("%c", POP);
        break;
      }
      case OP_SP: {
        *sp = (uint8_t*) sp - mem;
        sp++;
        break;
      }
      case OP_TOSP: {
        cell_t* new_sp = (cell_t*)(mem + POP);
        sp = new_sp;
        break;
      }
      case OP_DEPTH: {
        *sp = sp - (cell_t*)(mem + stack);
        sp++;
        break;
      }
      case OP_DP: {
        PUSH(dp - mem);
        break;
      }
      case OP_TODP: {
        dp = (uint8_t*) (mem + POP);
        break;
      }
      case OP_COMA: {
        cell_t val = POP;
        *dp = LO(val);
        *(dp +1) = HI(val);
        dp += 2;
        break;
      }
      case OP_CCOMA: {
        cell_t val = POP;
        *dp = LO(val);
        dp++;
        break;
      }
      case OP_SHL: {
        *(sp-2) <<= *(sp-1);
        sp--;
        break;
      }
      case OP_SAR: {
        *(sp-2) >>= *(sp-1);
        sp--;
        break;
      }
      case OP_RPUSH: {
        RPUSH(POP);
        break;
      }
      case OP_RPOP: {
        PUSH(RPOP);
        break;
      }
      case OP_HLT: {
        return POP;
      }
      case OP_NOP: {}
      default: {
        breach("Unknown opcode: 0x%x at ip=0x%x\n",
               code, ip - mem - start_ip);
        return 1;
      }
    }
  }
  return 0;
}
