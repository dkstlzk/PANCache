CXX := g++
CXXFLAGS := -std=c++17 -Iinclude -Wall -Wextra -O2
BUILD_DIR := build

# Source files
MAIN_SRC := src/main.cpp
HASHMAP_SRC := src/data/hashmap.cpp
LRU_SRC := src/data/lru.cpp
TEST_HASHMAP_SRC := tests/test_hashmap.cpp
TEST_LRU_SRC := tests/test_lru.cpp

# Binaries
MAIN_BIN := $(BUILD_DIR)/pancache_main.exe
TEST_HASHMAP_BIN := $(BUILD_DIR)/test_hashmap.exe
TEST_LRU_BIN := $(BUILD_DIR)/test_lru.exe

# Default target
all: $(MAIN_BIN) $(TEST_HASHMAP_BIN) $(TEST_LRU_BIN)

# --- Build rules ---

$(MAIN_BIN): $(MAIN_SRC) $(HASHMAP_SRC) $(LRU_SRC)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo Built main executable: $@

$(TEST_HASHMAP_BIN): $(HASHMAP_SRC) $(TEST_HASHMAP_SRC)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo Built HashMap test executable: $@

$(TEST_LRU_BIN): $(LRU_SRC) $(HASHMAP_SRC) $(TEST_LRU_SRC)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo Built LRU test executable: $@

# --- Utility targets ---

run: $(MAIN_BIN)
	@echo Running main program...
	@$(MAIN_BIN)

test: $(TEST_HASHMAP_BIN) $(TEST_LRU_BIN)
	@echo Running HashMap tests...
	@$(TEST_HASHMAP_BIN)
	@echo Running LRU tests...
	@$(TEST_LRU_BIN)

clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@echo Cleaned build directory.

.PHONY: all clean run test
