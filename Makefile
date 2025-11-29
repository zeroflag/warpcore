CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I. -O3

SRCS = $(filter-out main.c, $(wildcard *.c))
OBJS = $(SRCS:.c=.o)
OUT = warp

TEST_SRCS := $(wildcard test/*.c)
TEST_OBJS = $(TEST_SRC:.c=.o)
TEST_EXEC = test/run_tests
TEST_OUTP = test/out.txt

BOLD_GREEN = \033[1;32m
BOLD_RED   := \033[1;31m
RESET = \033[0m

all: clean vm test-core test-bootstrap

vm:
	$(CC) $(CFLAGS) -o $(OUT) $(SRCS) "main.c"

test-core:
	$(CC) $(CFLAGS) -o $(TEST_EXEC) $(TEST_SRCS) $(SRCS)
	./$(TEST_EXEC)

test-bootstrap:
	./test/test_bootstrap > $(TEST_OUTP)
	@diff -u ./test/expected.txt $(TEST_OUTP) || { \
		echo "$(BOLD_RED)✗ Bootstrap tests failed.$(RESET)"; \
		exit 1; \
	}
	@echo "$(BOLD_GREEN)✓ Bootstrap tests passed.$(RESET)"
	@rm -f $(TEST_OUTP)

clean:
	@rm -f $(OUT) $(OBJS) $(TEST_OBJS) $(TEST_EXEC) $(TEST_OUTP)

.PHONY: all clean test-core test-bootstrap
