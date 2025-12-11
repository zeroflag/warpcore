USE_MUSL ?= 0
OPT_LVL  ?= 1

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I. -I./io -O$(OPT_LVL) -march=native -funroll-loops

ifeq ($(USE_MUSL),1)
	CC = musl-gcc
	CFLAGS += -static -s 
endif

# SRCS = $(shell find . -name '*.c' ! -name 'main.c' ! -path './test/*')
SRCS = $(filter-out main.c, $(wildcard *.c))

OBJS = $(SRCS:.c=.o)
IMGS = $(wildcard *.img)
PYTHON = python
OUT = warp
STAGE1_IMG="stage1.img"
STAGE2_IMG="stage2.img"
BASIC_TESTS="test/test.forth"
TESTS_STAGE2="test/test_stage2.forth"

TEST_SRCS = $(wildcard test/*.c)
TEST_OBJS = $(TEST_SRC:.c=.o)
TEST_EXEC = test/run_tests
TEST_OUTP = test/out.txt
TEST_IMGS = $(wildcard test/*.img)

BOLD_GREEN = \033[1;32m
BOLD_RED   = \033[1;31m

CYAN  = \033[36m
RESET = \033[0m

all: clean vm test-vm test-bootstrap stage1 test-stage1 stage2 test-stage2

vm:
	@echo "* $(CYAN)Building vm..$(RESET)"
	$(CC) $(CFLAGS) $(SRCS) io/*.c main.c -o $(OUT) $(shell sdl2-config --cflags --libs)

stage1: vm
	@echo "* $(CYAN)Building stage1 compiler with bootstrap compiler..$(RESET)"
	@$(PYTHON) bootstrap.py compiler.forth $(STAGE1_IMG)

stage2: stage1
	@echo "* $(CYAN)Building stage2 compiler with stage1 compiler..$(RESET)"
	@cat lib.forth compiler.forth | ./$(OUT) $(STAGE1_IMG)
	@mv output.img $(STAGE2_IMG)

test-vm: vm
	@echo "* $(CYAN)Running VM tests..$(RESET)"
	$(CC) $(CFLAGS) -o $(TEST_EXEC) $(TEST_SRCS) $(SRCS)
	@./$(TEST_EXEC) || { \
		echo "$(BOLD_RED)✗ VM tests failed.$(RESET)"; \
		exit 1; \
	}
	@echo "$(BOLD_GREEN)✓ VM tests passed.$(RESET)"

test-bootstrap:
	@echo "* $(CYAN)Running bootstrap compiler tests..$(RESET)"
	@./test/test_bootstrap.sh > $(TEST_OUTP)
	@diff -u ./test/expected.txt $(TEST_OUTP) || { \
		echo "$(BOLD_RED)✗ Bootstrap compiler tests failed.$(RESET)"; \
		exit 1; \
	}
	@echo "$(BOLD_GREEN)✓ Bootstrap compiler tests passed.$(RESET)"
	@rm -f $(TEST_OUTP)

test-stage1: stage1
	@echo "* $(CYAN)Running stage1 compiler tests..$(RESET)"
	@COMPILER_IMAGE=$(STAGE1_IMG) TEST=$(BASIC_TESTS) ./test/test_compiler.sh > $(TEST_OUTP)
	@diff -u ./test/expected.txt $(TEST_OUTP) || { \
		echo "$(BOLD_RED)✗ Stage 1 compiler tests failed.$(RESET)"; \
		exit 1; \
	}
	@echo "$(BOLD_GREEN)✓ Stage 1 compiler tests passed.$(RESET)"
	@rm -f $(TEST_OUTP)

test-stage2: stage2
	@echo "* $(CYAN)Running stage2 compiler tests..$(RESET)"
	@COMPILER_IMAGE=$(STAGE2_IMG) TEST=$(BASIC_TESTS) ./test/test_compiler.sh > $(TEST_OUTP)
	@diff -u ./test/expected.txt $(TEST_OUTP) || { \
		echo "$(BOLD_RED)✗ Stage 2 compiler tests failed.$(RESET)"; \
		exit 1; \
	}
	@COMPILER_IMAGE=$(STAGE2_IMG) TEST=$(TESTS_STAGE2) ./test/test_compiler.sh > $(TEST_OUTP)
	@echo "$(BOLD_GREEN)✓ Stage 2 compiler tests passed.$(RESET)"
	@rm -f $(TEST_OUTP)

clean:
	@rm -f $(OUT) $(OBJS) $(IMGS) $(TEST_OBJS) $(TEST_EXEC) $(TEST_OUTP) $(TEST_IMGS)

.PHONY: all clean test-vm test-bootstrap test-stage1 test-stage2 stage1 stage2
