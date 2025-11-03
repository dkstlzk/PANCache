# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude

# Directories
SRC_DIR = src
BUILD_DIR = build
DATA_DIR = $(SRC_DIR)/data
TEST_DIR = tests

# Output executables
MAIN_TARGET = $(BUILD_DIR)/pancache_main
HASHMAP_TEST = $(BUILD_DIR)/test_hashmap
LRU_TEST = $(BUILD_DIR)/test_lru
INTEGRATION_TEST = $(BUILD_DIR)/test_integration

# Source files
MAIN_SRC = $(SRC_DIR)/main.cpp
DATA_SRC = $(DATA_DIR)/hashmap.cpp $(DATA_DIR)/lru.cpp

# Cross-platform mkdir command
MKDIR_P = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

# ---------- MAIN BUILD ----------
all: $(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_SRC) $(DATA_SRC)
	@$(MKDIR_P)
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) $(DATA_SRC) -o $(MAIN_TARGET)
	@echo "Built main executable: $(MAIN_TARGET)"

# ---------- TEST BUILDS ----------
$(HASHMAP_TEST): $(TEST_DIR)/test_hashmap.cpp $(DATA_DIR)/hashmap.cpp
	@$(MKDIR_P)
	$(CXX) $(CXXFLAGS) $(TEST_DIR)/test_hashmap.cpp $(DATA_DIR)/hashmap.cpp -o $(HASHMAP_TEST)
	@echo "Built HashMap test: $(HASHMAP_TEST)"

$(LRU_TEST): $(TEST_DIR)/test_lru.cpp $(DATA_DIR)/lru.cpp
	@$(MKDIR_P)
	$(CXX) $(CXXFLAGS) $(TEST_DIR)/test_lru.cpp $(DATA_DIR)/lru.cpp -o $(LRU_TEST)
	@echo "Built LRU test: $(LRU_TEST)"

$(INTEGRATION_TEST): $(TEST_DIR)/test_integration.cpp $(DATA_SRC)
	@$(MKDIR_P)
	$(CXX) $(CXXFLAGS) $(TEST_DIR)/test_integration.cpp $(DATA_SRC) -o $(INTEGRATION_TEST)
	@echo "Built Integration test (LRU + HashMap): $(INTEGRATION_TEST)"

# ---------- TEST RUNNERS ----------
test_hashmap: $(HASHMAP_TEST)
	@echo "Running HashMap test..."
	@$(HASHMAP_TEST)

test_lru: $(LRU_TEST)
	@echo "Running LRU test..."
	@$(LRU_TEST)

test_integration: $(INTEGRATION_TEST)
	@echo "Running Integration test (LRU + HashMap)..."
	@$(INTEGRATION_TEST)

# Run all tests
test_all: test_hashmap test_lru test_integration

# ---------- CLEAN ----------
clean:
	@if exist $(BUILD_DIR) (rmdir /s /q $(BUILD_DIR)) else (echo "No build directory found.")
	@echo "🧹 Cleaned build artifacts."
