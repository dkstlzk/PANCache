# Build System (Phase 1: HashMap)

CXX := g++
CXXFLAGS := -std=c++17 -Iinclude -Wall -Wextra -O2

# Directories
SRC_DIR := src
TEST_DIR := tests
BUILD_DIR := build

# Files
HASHMAP_SRC := $(SRC_DIR)/data/hashmap.cpp
MAIN_SRC := $(SRC_DIR)/main.cpp
HASHMAP_TEST := $(TEST_DIR)/test_hashmap.cpp

# Binaries
MAIN_BIN := $(BUILD_DIR)/pancache_main
HASHMAP_BIN := $(BUILD_DIR)/test_hashmap

# Default target
all: $(MAIN_BIN) $(HASHMAP_BIN)

# Build main program
$(MAIN_BIN): $(MAIN_SRC) $(HASHMAP_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built main executable: $@"


# Build HashMap unit test
$(HASHMAP_BIN): $(HASHMAP_SRC) $(HASHMAP_TEST)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built test executable: $@"

# Run unit tests
test: $(HASHMAP_BIN)
	@echo "Running HashMap unit tests..."
	@./$(HASHMAP_BIN)

# Run main program manually
run: $(MAIN_BIN)
	@echo "Running PANCache main..."
	@./$(MAIN_BIN)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory."

# Phony targets
.PHONY: all clean test run
