#!/bin/bash

OUT_IMAGE="output.img"
TEST="test/test.forth"
LIB="lib.forth"

rm -f "$OUT_IMAGE"

cat "$LIB"  \
    "$TEST" \
    | ./warp "$COMPILER_IMAGE" 

# run the emitted image
./warp "$OUT_IMAGE"
