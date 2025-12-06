CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I. -O3

SRCS = $(filter-out main.c, $(wildcard *.c))
OBJS = $(SRCS:.c=.o)
IMGS := $(wildcard *.img)
OUT = warp

TEST_SRCS := $(wildcard test/*.c)
TEST_OBJS = $(TEST_SRC:.c=.o)
TEST_EXEC = test/run_tests
TEST_OUTP = test/out.txt
TEST_IMGS := $(wildcard test/*.img)

BOLD_GREEN = \033[1;32m
BOLD_RED   := \033[1;31m
RESET = \033[0m

all: clean vm test-core test-bootstrap test-compiler test-self-hosted

vm:
	$(CC) $(CFLAGS) -o $(OUT) $(SRCS) "main.c"

test-core:
	$(CC) $(CFLAGS) -o $(TEST_EXEC) $(TEST_SRCS) $(SRCS)
	@./$(TEST_EXEC) || { \
		echo "$(BOLD_RED)✗ VM tests failed.$(RESET)"; \
		exit 1; \
	}
	@echo "$(BOLD_GREEN)✓ VM tests passed.$(RESET)"

test-bootstrap:
	@./test/test_bootstrap.sh > $(TEST_OUTP)
	@diff -u ./test/expected.txt $(TEST_OUTP) || { \
		echo "$(BOLD_RED)✗ Bootstrap compiler tests failed.$(RESET)"; \
		exit 1; \
	}
	@echo "$(BOLD_GREEN)✓ Bootstrap compiler tests passed.$(RESET)"
	@rm -f $(TEST_OUTP)

test-compiler:
	@./test/test_compiler.sh > $(TEST_OUTP)
	@diff -u ./test/expected.txt $(TEST_OUTP) || { \
		echo "$(BOLD_RED)✗ Stage 1 compiler tests failed.$(RESET)"; \
		exit 1; \
	}
	@echo "$(BOLD_GREEN)✓ Stage 1 compiler tests passed.$(RESET)"
	@rm -f $(TEST_OUTP)

test-self-hosted:
	@./test/test_self_hosted.sh > $(TEST_OUTP)
	@diff -u ./test/expected.txt $(TEST_OUTP) || { \
		echo "$(BOLD_RED)✗ Stage 2 compiler tests failed.$(RESET)"; \
		exit 1; \
	}
	@echo "$(BOLD_GREEN)✓ Stage 2 compiler tests passed.$(RESET)"
	@rm -f $(TEST_OUTP)

clean:
	@rm -f $(OUT) $(OBJS) $(IMGS) $(TEST_OBJS) $(TEST_EXEC) $(TEST_OUTP) $(TEST_IMGS)

.PHONY: all clean test-core test-bootstrap test-compiler test-self-hosted
