# ─── ToyLang Lexical Analyzer ── Makefile ───
CC      = gcc
CFLAGS  = -Wall -std=c99 -D_POSIX_C_SOURCE=200809L
TARGET  = toylang_lexer

# Directories
SRC_DIR  = src
TEST_DIR = tests

all: $(TARGET)

lex.yy.c: $(SRC_DIR)/toylang.l
	flex $(SRC_DIR)/toylang.l

$(TARGET): lex.yy.c $(SRC_DIR)/main.c $(SRC_DIR)/token.h
	$(CC) $(CFLAGS) -I$(SRC_DIR) -o $(TARGET) lex.yy.c $(SRC_DIR)/main.c -lfl

run: all
	./$(TARGET) $(TEST_DIR)/sample.toy

test-all: all
	@echo "========== TEST: sample.toy =========="
	@./$(TARGET) $(TEST_DIR)/sample.toy 2>&1 || true
	@echo ""
	@echo "========== TEST: keywords =========="
	@./$(TARGET) $(TEST_DIR)/test_keywords.toy 2>&1 || true
	@echo ""
	@echo "========== TEST: operators =========="
	@./$(TARGET) $(TEST_DIR)/test_operators.toy 2>&1 || true
	@echo ""
	@echo "========== TEST: errors =========="
	@./$(TARGET) $(TEST_DIR)/test_errors.toy 2>&1 || true
	@echo ""
	@echo "========== TEST: edge cases =========="
	@./$(TARGET) $(TEST_DIR)/test_edge_cases.toy 2>&1 || true

clean:
	rm -f lex.yy.c $(TARGET)

.PHONY: all run test-all clean
