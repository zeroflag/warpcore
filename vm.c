#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "vm.h"
#include "image.h"

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
              cell_t rstack)
{
  register cell_t*   sp = (cell_t*)   (mem + stack);
  register cell_t*   rp = (cell_t*)   (mem + rstack);
  register opcode_t* ip = (opcode_t*) (mem + start_ip);

  while (((uint8_t*)ip - mem) < MEM_SIZE) {
    opcode_t code = *ip++;
    #if DEBUG
      printf("[0x%X] OPCODE: 0x%X\n", (cell_t)(ip -1 - mem), code);
    #endif
    switch (code) {
      case OP_ADD:  BINARY(+=);   break;
      case OP_SUB:  BINARY(-=);   break;
      case OP_MUL:  BINARY(*=);   break;
      case OP_DIV:  BINARY(/=);   break;
      case OP_AND:  BINARY(&=);   break;
      case OP_OR:   BINARY(|=);   break;
      case OP_XOR:  BINARY(^=);   break;
      case OP_MOD:  BINARY(%=);   break;
      case OP_SHL:  BINARY(<<=);  break;
      case OP_SAR:  BINARY(>>=);  break;
      case OP_INC:  UNARY(+=, 1); break;
      case OP_DEC:  UNARY(-=, 1); break;
      case OP_EQ:   COMPARE(==);  break;
      case OP_NEQ:  COMPARE(!=);  break;
      case OP_LT:   COMPARE(<);   break;
      case OP_LTE:  COMPARE(<=);  break;
      case OP_GT:   COMPARE(>);   break;
      case OP_GTE:  COMPARE(>=);  break;
      case OP_INV:  NULLARY(~);   break;
      case OP_DUP:  PUSH(*(sp-1)); break;
      case OP_OVER: PUSH(*(sp-2)); break;
      case OP_NIP:  BINARY(=); break;
      case OP_DROP: sp--; break;
      case OP_SWAP: { // ( a b -- b a )
        cell_t tmp = *(sp-1);
        *(sp-1) = *(sp-2);
        *(sp-2) = tmp;
        break;
      }
      case OP_ROT: { // ( a b c -- b c a )
        cell_t tmp = *(sp-3);
        *(sp-3) = *(sp-2);
        *(sp-2) = *(sp-1);
        *(sp-1) = tmp;
        break;
      }
      case OP_TUCK: { // ( a b -- b a b )
        PUSH(*(sp-1));
        *(sp-2) = *(sp-3);
        *(sp-3) = *(sp-1);
        break;
      }
      case OP_LIT:
        PUSH(fetch_cell(ip));
        ip += sizeof(cell_t);
        break;
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
        opcode_t val = mem[POP];
        PUSH(val);
        break;
      }
      case OP_JZ:  JUMP_IF(POP == 0); break;
      case OP_JNZ: JUMP_IF(POP != 0); break;
      case OP_JMP: ip += fetch_cell(ip); break;
      case OP_CALL: {
        cell_t addr = (cell_t) ((uint8_t*)ip - mem + sizeof(cell_t));
        RPUSH(addr);
        SET_IP(fetch_cell(ip));
        break;
      }
      case OP_RET:  SET_IP(RPOP); break;
      case OP_KEY:  PUSH(getchar());   break;
      case OP_EMIT: printf("%c", POP); break;
      case OP_SP:   PUSH((uint8_t*) sp - mem); break;
      case OP_TOSP: {
        cell_t* new_sp = (cell_t*)(mem + POP);
        sp = new_sp;
        break;
      }
      case OP_RPUSH: RPUSH(POP); break;
      case OP_RPOP:  PUSH(RPOP); break;
      case OP_RTOP:  PUSH(*(rp-1)); break;
      case OP_BTICK: PUSH(*ip++); break;
      case OP_CTICK:
        if (*ip++ != OP_CALL)
          breach("Expected OP_CALL, got: %x\n", *(ip-1));
        PUSH(fetch_cell(ip));
        ip += sizeof(cell_t);
        break;
      case OP_HLT: return POP;
      case OP_ABORT:
        breach("ABORTED: ip=0x%x\n", ip - mem);
        break;
      case OP_DUMP: {
        char *path = (char *)(POP + mem);
        PUSH((cell_t)dump_image(mem, path));
        break;
      }
      default:
        breach("Unknown opcode: 0x%x at ip=0x%x\n",
               code, ip - mem);
        return 1;
    }
  }
  return 0;
}
