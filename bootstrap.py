import re, sys
from io import StringIO

HEADER = "vm.h"

word_matcher = re.compile(
  "\s*(OP_\w+)\s*=\s*(0x[0-9A-Fa-f]+),\s*//\s*\"(\S+)\"")

primitives = {}
words = {}
macros = {}
stack = []

SIZE = 32767
CELL_MAX =  65535
CELL_MIN = -32768
COMPILER_ENTRY = 0x6000
VAR_DP = 0x182

jmp_address = 0
dp = 0
mem = [0 for i in range(SIZE)]

VM_PARAM_ADDRESS = 0x42
DEFAULT_FILENAME = "output.img"

class Tokens:
  @staticmethod
  def parse_file(input_file):
    with open(input_file) as f:
      return Tokens(list(f.read()))

  def __init__(self, stream):
    self.stream = stream

  def prepend(self, text):
    self.stream[:0] = list(text)

  def has_more(self):
    return len(self.stream) > 0

  def next(self):
    ch = self.next_char()
    while ch.isspace():
      ch = self.next_char()
    token = StringIO()
    while not ch.isspace():
      token.write(ch)
      ch = self.next_char()
    return token.getvalue()

  def read_until(self, end_ch):
    token = StringIO()
    ch = self.next_char()
    while ch != end_ch:
      token.write(ch)
      ch = self.next_char()
    return token.getvalue()

  def next_char(self):
    return self.stream.pop(0)

class StringPool:
  def __init__(self):
    self.pool = {}

  def get(self, s):
    if s in self.pool:
      return self.pool[s]
    else:
      return None

  def add(self, s, address):
    if s not in self.pool:
      self.pool[s] = address

def push(val): stack.append(val)
def pop(): return stack.pop()
def tos(): return stack[-1]
def swap(): stack[-1], stack[-2] = stack[-2], stack[-1]
def inc2nd(): stack[-2] += 1

def read_primitives():
  with open(HEADER) as f:
    for line in f.readlines():
      m = word_matcher.match(line)
      if m:
        primitives[m.group(3)] = int(m.group(2), 16)

def tonum(token):
  if token.lower().startswith("$"):
    base = 16
    token = token[1:]
  elif token.lower().startswith("%"):
    base = 2
    token = token[1:]
  else:
    base = 10
  return int(token, base)

def compile_token(token):
  #print("[0x%X] Compiling %s" % (dp, token))
  if token in words:
    compile_call(words[token])
  elif token in primitives:
    compile_primitive(token)
  elif token in macros:
    macros[token]()
  else:
    compile_lit(tonum(token))

def compile_call(address):
  compile_primitive("CALL")
  compile_num16(address)

def compile_primitive(opcode):
  global dp
  mem[dp] = primitives[opcode]
  dp += 1

def compile_lit(num):
  compile_primitive('LIT')
  compile_num16(num)

def compile_num16(num):
  global dp
  if num > CELL_MAX or num < CELL_MIN:
    raise RuntimeError("Number out of range: %d" % num)
  mem[dp] = num & 0xFF
  dp += 1
  mem[dp] = (num >> 8) & 0xFF
  dp += 1

def compile_num8(num):
  global dp
  if num > 127 or num < -128:
    raise RuntimeError("Number out of range: %d" % num)
  mem[dp] = num & 0xFF
  dp += 1

def compile(tokens):
  while tokens.has_more():
    compile_token(tokens.next())

def dump(mem, output):
  with open(output, 'wb') as f:
    f.write(bytearray(mem))

def compile_forward_jump(branch_type):
  compile_primitive(branch_type)
  push(dp)
  compile_num16(0)

def compile_back_jump(jump_type):
  compile_primitive(jump_type)
  compile_num16(pop() - dp)

def fill_branch_address():
  address = pop()
  offset = dp - address
  if offset > CELL_MAX or offset < CELL_MIN:
    raise RuntimeError("Relative address out of range: %d" % offset)
  mem[address] = offset & 0xFF
  mem[address +1] = (offset >> 8) & 0xFF

def def_var(name):
  # LIT @SLOT EXIT SLOT
  global dp
  def_word(name)
  compile_lit(dp + 4) # after EXIT
  compile_primitive("EXIT")
  dp += 2

def def_word(name):
  # print("Defining word: %s" % name)
  if name in words:
    print("Warning: redefined %s" % name)
  words[name] = dp

def compile_string(s):
  if pool.get(s):
    compile_lit(pool.get(s))
  else:
    pool.add(s, dp + 6)
    compile_lit(dp + 6)
    compile_forward_jump("JMP")
    for i in s:
      compile_num8(ord(i))
    compile_num8(0)
    fill_branch_address()

def compile_entry(address):
  mem[jmp_address] = address & 0xFF
  mem[jmp_address+1] = (address >> 8) & 0xFF

def create_macros():
  macros["IF"] = lambda: compile_forward_jump("JZ")
  macros["THEN"] = fill_branch_address
  macros["ELSE"] = lambda: (compile_forward_jump("JMP"),
                            swap(),
                            fill_branch_address())
  macros["BEGIN"] = lambda: push(dp)
  macros["UNTIL"] = lambda: compile_back_jump("JZ")
  macros["AGAIN"] = lambda: compile_back_jump("JMP")
  macros["WHILE"] = lambda: compile_forward_jump("JZ")
  macros["REPEAT"] = lambda: (swap(),
                              compile_back_jump("JMP"),
                              fill_branch_address())
  macros["VARIABLE"] = lambda: def_var(tokens.next())
  macros["CASE"] = lambda: push(0)
  macros["ENDCASE"] = lambda: [fill_branch_address() for i in range(pop())]
  macros["OF"] = lambda: (compile_primitive('OVER'),
                          compile_primitive('='),
                          compile_forward_jump("JZ"),
                          compile_primitive("DROP"))
  macros["ENDOF"] = lambda: (inc2nd(),
                             compile_forward_jump("JMP"),
                             swap(),
                             fill_branch_address(),
                             swap()) # keep counter on top
  macros["FOR"] = lambda: (compile_primitive(">R"),
                           push(dp))
  macros["NEXT"] = lambda: (compile_primitive("R@"),
                            compile_forward_jump("JZ"),
                            compile_primitive("R>"),
                            compile_lit(1),
                            compile_primitive("-"),
                            compile_primitive(">R"),
                            swap(),
                            compile_back_jump("JMP"),
                            fill_branch_address(),
                            compile_primitive("R>"),
                            compile_primitive("DROP"))
  macros[":"] = lambda: def_word(tokens.next())
  macros[";"] = lambda: compile_primitive("EXIT")
  macros['"'] = lambda: compile_string(tokens.read_until('"'))
  macros['('] = lambda: tokens.read_until(')')
  macros['\\'] =  lambda: tokens.read_until("\n")
  macros["ENTRY"] = lambda: compile_entry(dp)
  macros["BYE"] = lambda: 0
  macros["LITERAL"] = lambda: (compile_primitive('LIT'),
                               compile_token(','))
  macros["I"] = lambda: compile_primitive("R@")
  macros["CHAR"] = lambda: compile_lit(ord(tokens.next()))
  macros["DP"] = lambda: compile_lit(VAR_DP)
  macros["EXEC"] = lambda: (compile_primitive(">R"),
                         compile_primitive("EXIT"))

def make_header():
  global dp, jmp_address
  mem[dp] = 0b00010000 # version
  dp += 1
  mem[dp] = 0xFF
  dp = 0x190
  jmp_address = dp + 1
  compile_lit(0xFF) # placeholder JMP address: 0x183-0x184
  compile_primitive(">R")
  compile_primitive("EXIT")
  dp = COMPILER_ENTRY
  for i in range(len(DEFAULT_FILENAME)):
    mem[VM_PARAM_ADDRESS + i] = ord(DEFAULT_FILENAME[i])

DEFS = """
: MAIN $0190 ;
: TRUE -1 ;
: FALSE 0 ;

: KEY  2 IN  ;
: EMIT 1 OUT ;
: C!
  DUP @ $FF00 AND
  ROT $FF AND OR
  SWAP ! ;
: C@  @ $FF AND ;
: ++ DUP @ 1+ SWAP ! ;

: ,   DP @   ! DP ++ DP ++ ;
: C,  DP @  C! DP ++ ;

: CODE R>    DUP C@ SWAP 1+  >R C, ;
: SUB  R> 1+ DUP  @ SWAP 2 + >R  , ;

: ?DUP  DUP 0 <> IF DUP THEN ;
: 2DUP OVER OVER ;
: 2DROP DROP DROP ;
: 3DROP DROP DROP DROP ;
: CR 10 EMIT ;
: -ROT ROT ROT ;
: TUCK DUP ROT ROT ;

: NON-ZERO? C@ 0 <> ;
: PRINT
  BEGIN
    DUP NON-ZERO?
  WHILE
    DUP C@ EMIT
    1 +
  REPEAT
  DROP ;

: /MOD 2DUP % -ROT / ;
: DEPTH SP $02 - ;

: .
    DUP 0 < IF 45 EMIT -1 * THEN
    10 /MOD ?DUP IF . THEN
    48 + EMIT ;

: BETWEEN? OVER >= -ROT <= AND ;

: C= C@ SWAP C@ = ;

: STR=
  BEGIN
    2DUP C=
    OVER NON-ZERO? AND
    OVER NON-ZERO? AND
  WHILE
    1+ SWAP 1+
  REPEAT
  C= ;

: STRLEN
  0
  BEGIN
    OVER NON-ZERO?
  WHILE
    1+ SWAP
    1+ SWAP
  REPEAT
  NIP ;

VARIABLE BASE

: NEG? C@ CHAR - = ;
: HEX? C@ CHAR $ = ;
: BIN? C@ CHAR % = ;

: 0..9? CHAR 0 SWAP CHAR 9 BETWEEN? ;

: >DIGIT
  C@ DUP 0..9? IF
    CHAR 0 -
  ELSE
    CHAR A - 10 +
  THEN ;

: DIGIT?
  BASE @ 10 <= IF
    CHAR 0 SWAP C@ 47 BASE @ + BETWEEN?
    EXIT
  THEN
  BASE @ 16 <= IF
    C@ DUP 0..9?
    CHAR A ROT 54 BASE @ + BETWEEN?
    OR
    EXIT
  THEN
  FALSE ;
: >NUMBER ( s -- n bool )
  DUP HEX? IF
    1+ 16
  ELSE
    DUP BIN? IF 1+ 2 ELSE 10 THEN
  THEN
  BASE !
  DUP NEG? IF 1+ -1 ELSE 1 THEN
  SWAP 0
  ( sign str result )
  BEGIN
    OVER NON-ZERO?
  WHILE
    OVER DIGIT? INVERT IF 3DROP FALSE EXIT THEN
    BASE @ * OVER >DIGIT +
    SWAP 1+ SWAP
  REPEAT
  NIP * TRUE ;

: MIN OVER OVER < IF DROP ELSE NIP  THEN ;
: MAX OVER OVER < IF NIP  ELSE DROP THEN ;
"""

if __name__ == "__main__":
  read_primitives()

  input_file = sys.argv[1]
  output_file = sys.argv[2]

  pool = StringPool()
  tokens = Tokens.parse_file(input_file)
  tokens.prepend(DEFS)
  create_macros()
  make_header()
  compile(tokens)
  #print("Writing output: %s" % output_file)
  dump(mem, output_file)
