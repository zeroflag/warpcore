#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "vm.h"

const cell_t MEM_SIZE = SHRT_MAX;

void breach(char* format, ...) {
  va_list args;
  printf("[ERROR]: ");
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  exit(1);
}

cell_t engage(char *mem,
              cell_t start_ip,
              cell_t stack,
              cell_t rstack)
{
  cell_t*   sp = (cell_t*) (mem + stack);
  cell_t*   rp = (cell_t*) (mem + rstack);
  opcode_t* ip = (opcode_t*) (mem + start_ip);

  while (((char*)ip - mem) < MEM_SIZE) {
    opcode_t code = *ip++;
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
      case OP_JZ: {
        if (POP == 0) {
          ip += (int8_t)*ip;
        } else {
          ip += sizeof(int8_t);
        }
        break; 
      }
      case OP_JNZ: {
        if (POP != 0) {
          ip += (int8_t)*ip;
        } else {
          ip += sizeof(int8_t);
        }
        break; 
      }
      case OP_JMP: {
        ip += (int8_t)*ip;
        break;
      }
      case OP_CALL: {
        RPUSH((cell_t) (mem - (char*)ip + sizeof(uint16_t)));
        uint16_t address = ((uint16_t)*ip) | ((uint16_t)(*ip+1) << 8);
        ip = (opcode_t *) (mem + address);
        break;
      }
      case OP_RET: {
        cell_t address = RPOP;
        ip = (opcode_t *) (mem + address);
        break;
      }
      case OP_LT: {
        *(sp-2) = *(sp-2) < *(sp-1) ? -1 : 0;
        sp--;
        break;
      }
      case OP_GT: {
        *(sp-2) = *(sp-2) > *(sp-1) ? -1 : 0;
        sp--;
        break;
      }
      case OP_INV: {
        *(sp-1) = ~ *(sp-1);
        break;
      }
      case OP_OR: {
        *(sp-2) |= *(sp-1);
        sp--;
        break;
      }
      case OP_AND: {
        *(sp-2) &= *(sp-1);
        sp--;
        break;
      }
      case OP_PRN: {
        printf("%d", POP);
        break;
      }
      case OP_CR: {
        printf("\n");
        break;
      }
      case OP_DPT: {
        *sp = sp - (cell_t*)(mem + stack);
        sp++;
        break;
      }
      case OP_HLT: {
        return POP;
      }
      default: {
        breach("Unknown opcode: %x\n", code);
        return 1;
      }
    }
  }
  return 0;
}
