CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I. -O3

SRCS = $(filter-out main.c, $(wildcard *.c))
OBJS = $(SRCS:.c=.o)
OUT = warp

TEST_SRCS := $(wildcard test/*.c)
TEST_OBJS = $(TEST_SRC:.c=.o)
TEST_OUT = test/run_tests

all: clean vm test

vm:
	$(CC) $(CFLAGS) -o $(OUT) $(SRCS) "main.c"

test:
	$(CC) $(CFLAGS) -o $(TEST_OUT) $(TEST_SRCS) $(SRCS)
	./$(TEST_OUT)

clean:
	rm -f $(OUT) $(OBJS) $(TEST_OBJS) $(TEST_OUT)

.PHONY: all clean test
