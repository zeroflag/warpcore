#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include "vm.h"
#include "image.h"

#define DEBUG 0

#define PORT_STDOUT 1
#define PORT_STDIN  2
#define PORT_RND    3

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

inline cell_t fetch_cell(const uint8_t* addr) {
  return (cell_t) ((uint16_t) *addr | ((uint16_t) *(addr+1) << 8));
}

inline cell_t mem_fetch(cell_t addr, uint8_t *mem) {
  return (uint16_t)mem[addr] |
             ((uint16_t)mem[addr +1] << 8);
}

inline void mem_store(cell_t addr, cell_t val, uint8_t *mem) {
  mem[addr] = LO(val);
  mem[addr +1] = HI(val);
}

inline void swap(cell_t* sp) {
  cell_t tmp = *(sp-1);
  *(sp-1) = *(sp-2);
  *(sp-2) = tmp; 
}

inline void rot(cell_t* sp) {
  cell_t tmp = *(sp-3);
  *(sp-3) = *(sp-2);
  *(sp-2) = *(sp-1);
  *(sp-1) = tmp;
}

inline cell_t in(cell_t port) {
  switch (port) {
    case PORT_STDIN:
      return getchar();
    case PORT_RND:
      return (cell_t)(rand() % 65536 - 32768);
    default:
      breach("Invalid input port number: %d\n", port);
      return 0;
  }
}

inline void out(cell_t port, cell_t data) {
  switch (port) {
    case PORT_STDOUT:
      putchar(data);
      break;
    default:
      breach("Invalid output port number: %d\n", port);
  }
}

cell_t engage(uint8_t *mem,
              cell_t start_ip,
              cell_t stack,
              cell_t rstack)
{
  register cell_t*  sp = SET_SP(stack);
  register cell_t*  rp = SET_RP(rstack);
  register uint8_t* ip = SET_IP(start_ip);

  while (((uint8_t*)ip - mem) < MEM_SIZE) {
    uint8_t code = *ip++;
    #if DEBUG
      printf("[0x%X] OPCODE: 0x%X\n", (cell_t)(ip -1 - mem), code);
    #endif
    switch (code) {
      case OP_ADD:   BINARY(+=);           break;
      case OP_SUB:   BINARY(-=);           break;
      case OP_MUL:   BINARY(*=);           break;
      case OP_DIV:   BINARY(/=);           break;
      case OP_AND:   BINARY(&=);           break;
      case OP_OR:    BINARY(|=);           break;
      case OP_XOR:   BINARY(^=);           break;
      case OP_MOD:   BINARY(%=);           break;
      case OP_SHL:   BINARY(<<=);          break;
      case OP_SAR:   BINARY(>>=);          break;
      case OP_INC:   UNARY(+=, 1);         break;
      case OP_DEC:   UNARY(-=, 1);         break;
      case OP_EQ:    COMPARE(==);          break;
      case OP_NEQ:   COMPARE(!=);          break;
      case OP_LT:    COMPARE(<);           break;
      case OP_LTE:   COMPARE(<=);          break;
      case OP_GT:    COMPARE(>);           break;
      case OP_GTE:   COMPARE(>=);          break;
      case OP_INV:   NULLARY(~);           break;
      case OP_DUP:   PUSH(*(sp-1));        break;
      case OP_OVER:  PUSH(*(sp-2));        break;
      case OP_NIP:   BINARY(=);            break;
      case OP_DROP:  sp--;                 break;
      case OP_SWAP:  swap(sp);             break;
      case OP_ROT:   rot(sp);              break;
      case OP_JZ:    JUMP_IF(POP == 0);    break;
      case OP_JMP:   ip += fetch_cell(ip); break;
      case OP_RET:   SET_IP(RPOP);         break;
      case OP_RPUSH: RPUSH(POP);           break;
      case OP_RPOP:  PUSH(RPOP);           break;
      case OP_RTOP:  PUSH(*(rp-1));        break;
      case OP_HLT:   return POP;
      case OP_SP:    PUSH((uint8_t*) sp - mem);
                     break;
      case OP_LIT:   PUSH(fetch_cell(ip));
                     ip += sizeof(cell_t);
                     break;
      case OP_ABORT: breach("ABORTED: ip=0x%x\n", ip - mem);
                     break;
      case OP_DUMP:  dump_image(mem, (char *)(POP + mem));
                     break;
      case OP_IN: {
        cell_t port = POP;
        PUSH(in(port));
        break;
      }
      case OP_OUT: {
        cell_t port = POP;
        cell_t data = POP;
        out(port, data);
        break;
      }
      case OP_STOR: {
        cell_t addr = POP;
        cell_t val  = POP;
        mem_store(addr, val, mem);
        break;
      }
      case OP_LOAD: {
        cell_t addr = POP;
        PUSH(mem_fetch(addr, mem));
        break;
      }
      case OP_CALL: {
        cell_t addr = (cell_t) ((uint8_t*)ip - mem + sizeof(cell_t));
        RPUSH(addr);
        SET_IP(fetch_cell(ip));
        break;
      }
      default:
        breach("Unknown opcode: 0x%x at ip=0x%x\n",
               code, ip -1 - mem);
    }
  }
  return 0;
}
