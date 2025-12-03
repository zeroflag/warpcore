#!/bin/bash

COMPILER_IMAGE="compiler.img"
COMPILER_FORTH="compiler.forth"
OUT_IMAGE="output.img"
TEST="test/smoke.forth"

rm -f "$IMAGE" "$OUT_IMAGE"

# build compiler image
python bootstrap.py "$COMPILER_FORTH" "$COMPILER_IMAGE"

# compile tests using the compiler image
./warp "$COMPILER_IMAGE" < "$TEST"

# run the emitted image
./warp "$OUT_IMAGE"
