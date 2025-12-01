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
CELL_MAX =  32767
CELL_MIN = -32768

dp = 0
mem = [0 for i in range(SIZE)]

class Tokens:
  @staticmethod
  def parse_file(input_file):
    with open(input_file) as f:
      return Tokens(list(f.read()))

  def __init__(self, stream):
    self.stream = stream

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
  if token.lower().startswith("0x"):
    base = 16
  elif token.lower().startswith("0b"):
    base = 2
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
  compile_num8(0)

def compile_back_jump(jump_type):
  compile_primitive(jump_type)
  compile_num8(pop() - dp)

def fill_branch_address():
  address = pop()
  rel = dp - address
  if rel > 127 or rel < -128:
    raise RuntimeError("Relative address out of range: %d" % rel)
  mem[address] = rel

def compile_jump_address():
  compile_num8(pop() - dp)

def def_const(name, val):
  def_word(name)
  compile_lit(tonum(val))
  compile_primitive("EXIT")

def def_var(name):
  # LIT @SLOT EXIT SLOT
  global dp
  def_word(name)
  compile_lit(dp + 4) # after EXIT
  compile_primitive("EXIT")
  dp += 2

def def_word(name):
  # print("Defining word: %s" % name)
  words[name] = dp

def compile_string():
  s = tokens.read_until('"')
  if pool.get(s):
    compile_lit(pool.get(s))
  else:
    pool.add(s, dp + 5)
    compile_lit(dp + 5)
    compile_forward_jump("JMP")
    for i in s:
      compile_num8(ord(i))
    compile_num8(0)
    fill_branch_address()

def compile_entry(address):
  mem[2] = address & 0xFF
  mem[3] = (address >> 8) & 0xFF

def skip_until(end):
  tok = tokens.next()
  while tok != end:
    tok = tokens.next()

def create_macros():
  global dp, tokens
  macros["IF"] = lambda: compile_forward_jump("JZ")
  macros["THEN"] = lambda: fill_branch_address()
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
  macros["CONSTANT"] = lambda: def_const(tokens.next(), tokens.next())
  macros["VARIABLE"] = lambda: def_var(tokens.next())
  macros["CASE"] = lambda: push(0)
  macros["ENDCASE"] = lambda: [fill_branch_address() for i in range(pop())]
  macros["OF"] = lambda: (compile_primitive('OVER'),
                          compile_primitive('='),
                          compile_forward_jump("JZ"),
                          compile_primitive('DROP'))
  macros["ENDOF"] = lambda: (inc2nd(),
                             compile_forward_jump("JMP"),
                             swap(),
                             fill_branch_address(),
                             swap()) # keep counter on top
  macros[":"] = lambda: def_word(tokens.next())
  macros[";"] = lambda: compile_primitive("EXIT")
  macros['s"'] = lambda: compile_string()
  macros['('] = lambda: skip_until(")")
  macros["ENTRY"] = lambda: compile_entry(dp)

def make_header():
  global dp
  entry = 0x164
  mem[dp] = 0b00010000 # version
  dp += 1
  compile_primitive("LJMP")
  compile_num16(entry)
  dp = entry

if __name__ == "__main__":
  read_primitives()

  if len(sys.argv) != 3:
    for name, opcode in primitives.items():
      sep = " " * (12 - len(name))
      print("# %s%s0x%0.2X   PRIMITIVE" % (name, sep, opcode))
    sys.exit()

  input_file = sys.argv[1]
  output_file = sys.argv[2]

  pool = StringPool()
  tokens = Tokens.parse_file(input_file)
  create_macros()
  make_header()
  compile(tokens)
  #print("Writing output: %s" % output_file)
  dump(mem, output_file)
