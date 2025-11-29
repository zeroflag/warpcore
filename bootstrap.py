import re, sys

HEADER = "vm.h"

word_matcher = re.compile(
  "\s*(OP_\w+)\s*=\s*(0x[0-9A-Fa-f]+),\s*//\s*\"(\S+)\"")

primitives = {}
words = {}
macros = {}
stack = []
tokens = []

def push(val): stack.append(val)
def pop(): return stack.pop()
def tos(): return stack[-1]
def swap(): stack[-1], stack[-2] = stack[-2], stack[-1]

SIZE = 32767
CELL_MAX =  32767
CELL_MIN = -32768

dp = 0
mem = [0 for i in range(SIZE)]

def read_primitives():
  with open(HEADER) as f:
    for line in f.readlines():
      m = word_matcher.match(line)
      if m:
        primitives[m.group(3)] = int(m.group(2), 16)

def tonum(token):
  if token.lower().startswith("0x"):
    base = 16
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
    raise("Number out of range: %d" % num)
  mem[dp] = num & 0xFF
  dp += 1
  mem[dp] = (num >> 8) & 0xFF
  dp += 1

def compile_num8(num):
  global dp
  if num > 127 or num < -128:
    raise("Number out of range: %d" % num)
  mem[dp] = num & 0xFF
  dp += 1

def parse(source):
  return source.split()

def compile(tokens):
  while tokens:
    compile_token(tokens.pop(0))

def dump(mem, output):
  with open(output, 'wb') as f:
    f.write(bytearray(mem))

def compile_branch(branch_type):
  global dp
  compile_primitive(branch_type)
  push(dp)
  compile_num8(0)

def fill_branch_address():
  global dp
  address = pop()
  mem[address] = dp - address

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

def compile_entry(address):
  mem[2] = address & 0xFF
  mem[3] = (address >> 8) & 0xFF
  
def create_macros():
  global dp, tokens
  macros["IF"] = lambda: compile_branch("JZ")
  macros["THEN"] = lambda: fill_branch_address()
  macros["ELSE"] = lambda: (compile_branch("JMP"),
                            swap(),
                            fill_branch_address())
  macros["BEGIN"] = lambda: push(dp)
  macros["UNTIL"] = lambda: (compile_primitive("JZ"), # XXX
                             compile_num8(pop() - dp))

  macros["CONSTANT"] = lambda: def_const(tokens.pop(0), tokens.pop(0))
  macros["VARIABLE"] = lambda: def_var(tokens.pop(0))
  macros[":"] = lambda: def_word(tokens.pop(0))
  macros[";"] = lambda: compile_primitive("EXIT")
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
  input_file = sys.argv[1]
  output_file = sys.argv[2]
  with open(input_file) as f:
    read_primitives()
    create_macros()
    make_header()
    tokens = parse(f.read())
    compile(tokens)
    #print("Writing output: %s" % output_file)
    dump(mem, output_file)
