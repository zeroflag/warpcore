import re, sys

HEADER = "vm.h"

word_matcher = re.compile(
  "\s*(OP_\w+)\s*=\s*(0x[0-9A-Fa-f]+),\s*//\s*\"(\S+)\"")

primitives = {}
words = {}
macros = {}
stack = []

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

def compile_token(token):
  print("compiling %s" % token)
  if token in words:
    compile_word(words[token])
  elif token in primitives:
    compile_primitive(token)
  elif token in macros:
    macros[token]()
  else:
    compile_lit(int(token))

def compile_word(address):
  pass

def compile_primitive(opcode):
  global dp
  mem[dp] = primitives[opcode]
  dp += 1

def compile_lit(num):
  global dp
  if num > CELL_MAX or num < CELL_MIN:
    raise("Number out of range: %d" % num)
  compile_primitive('LIT')
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
  for tok in tokens:
    compile_token(tok)

def dump(mem, output):
  with open(output, 'wb') as f:
    f.write(bytearray(mem))


def create_macros():
  global dp
  def macro_if():
    compile_primitive("JZ")
    push(dp)
    compile_num8(0)

  def macro_else():
    compile_primitive("JMP")
    push(dp)
    compile_num8(0)
    swap()
    address = pop()
    mem[address] = dp - address

  def macro_then():
    address = pop()
    mem[address] = dp - address

  macros["IF"] = macro_if
  macros["THEN"] = macro_then
  macros["ELSE"] = macro_else
    
if __name__ == "__main__":
  input_file = sys.argv[1]
  output_file = sys.argv[2]
  with open(input_file) as f:
    read_primitives()
    create_macros()
    compile(parse(f.read()))
    print("Writing output: %s" % output_file)
    dump(mem, output_file)
