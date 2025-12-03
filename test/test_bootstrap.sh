#!/bin/bash

IMAGE="test/bootstrap_test.img"
TEST="test/test.forth"

rm -f "$IMAGE"

python bootstrap.py "$TEST" "$IMAGE"
./warp "$IMAGE"
