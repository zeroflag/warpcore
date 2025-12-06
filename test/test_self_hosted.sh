#!/bin/bash

STAGE1_IMAGE="stage1.img"
STAGE2_IMAGE="stage2.img"

COMPILER_FORTH="compiler.forth"
OUT_IMAGE="output.img"

TEST="test/test.forth"
LIB="lib.forth"

rm -f "$STAGE1_IMAGE" "$STAGE2_IMAGE" "$OUT_IMAGE"

# TODO DP hack remove later
sed -i 's/^: START-IP .*/: START-IP $0164 ;/' compiler.forth

# build stage 1 compiler
python bootstrap.py "$COMPILER_FORTH" "$STAGE1_IMAGE"

# TODO DP hack remove later
sed -i 's/^: START-IP .*/: START-IP $5000 ;/' compiler.forth
# bouild stage2: outputs outpout.img
cat "$LIB"  \
    "$COMPILER_FORTH" \
    | ./warp "$STAGE1_IMAGE"

# TODO DP hack remove later
sed -i 's/^: START-IP .*/: START-IP $0164 ;/' compiler.forth

mv "$OUT_IMAGE" "$STAGE2_IMAGE"

# compile tests using the stage2
cat "$LIB"  \
    "$TEST" \
    | ./warp "$STAGE2_IMAGE"

# run the emitted image
./warp "$OUT_IMAGE"
