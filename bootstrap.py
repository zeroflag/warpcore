import re, sys

HEADER = "vm.h"

word_matcher = re.compile(
  "\s*(OP_\w+)\s*=\s*(0x[0-9A-Fa-f]+),\s*//\s*\"(\S+)\"")

primitives = {}
words = {}

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
    compile_primitive(primitives[token])
  else:
    compile_number(int(token))

def compile_word(address):
  pass

def compile_primitive(opcode):
  global dp
  mem[dp] = opcode
  dp += 1

def compile_number(num):
  global dp
  if num > CELL_MAX or num < CELL_MIN:
    raise("Number out of range: %d" % num)
  compile_primitive(primitives['LIT'])
  mem[dp] = num & 0xFF
  dp += 1
  mem[dp] = (num >> 8) & 0xFF
  dp += 1

def parse(source):
  return source.split()

def compile(tokens):
  for tok in tokens:
    compile_token(tok)

def dump(mem, output):
  with open(output, 'wb') as f:
    f.write(bytearray(mem))
    
if __name__ == "__main__":
  input_file = sys.argv[1]
  output_file = sys.argv[2]
  with open(input_file) as f:
    read_primitives()
    compile(parse(f.read()))
    print("Writing output: %s" % output_file)
    dump(mem, output_file)
