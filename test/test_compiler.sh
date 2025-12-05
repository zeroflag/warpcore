#!/bin/bash

COMPILER_IMAGE="compiler.img"
COMPILER_FORTH="compiler.forth"
OUT_IMAGE="output.img"
TEST="test/test.forth"
LIB="lib.forth"

rm -f "$IMAGE" "$OUT_IMAGE"

# build compiler image
python bootstrap.py "$COMPILER_FORTH" "$COMPILER_IMAGE"

# compile tests using the compiler image
cat "$LIB"  \
    "$TEST" \
    | ./warp "$COMPILER_IMAGE" 

# run the emitted image
./warp "$OUT_IMAGE"
