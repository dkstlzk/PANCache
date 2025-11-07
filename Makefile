# =============================
# PANCache Build System
# =============================

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude

# --- Directories ---
SRC_DIR := src
DATA_DIR := $(SRC_DIR)/data
TEST_DIR := tests
BUILD_DIR := build

# --- Source files ---
MAIN_SRC := $(SRC_DIR)/main.cpp
DATA_SRC := $(DATA_DIR)/hashmap.cpp $(DATA_DIR)/lru.cpp $(DATA_DIR)/ttl_heap.cpp

# --- Targets ---
MAIN_TARGET := $(BUILD_DIR)/pancache_main
HASHMAP_TEST := $(BUILD_DIR)/test_hashmap
LRU_TEST := $(BUILD_DIR)/test_lru
TTL_TEST := $(BUILD_DIR)/test_heap
INTEGRATION_TEST := $(BUILD_DIR)/test_integration

# --- Ensure build directory exists ---
MKDIR_P = mkdir -p $(BUILD_DIR)

# =============================
# Build Targets
# =============================

all: $(MAIN_TARGET) $(HASHMAP_TEST) $(LRU_TEST) $(TTL_TEST) $(INTEGRATION_TEST)

# ---------- MAIN EXECUTABLE ----------
$(MAIN_TARGET): $(MAIN_SRC) $(DATA_SRC)
	@$(MKDIR_P)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "✅ Built main executable: $(MAIN_TARGET)"

# ---------- HASHMAP TEST ----------
$(HASHMAP_TEST): $(DATA_DIR)/hashmap.cpp $(TEST_DIR)/test_hashmap.cpp
	@$(MKDIR_P)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "🧩 Built HashMap test: $(HASHMAP_TEST)"

# ---------- LRU TEST ----------
$(LRU_TEST): $(DATA_DIR)/hashmap.cpp $(DATA_DIR)/lru.cpp $(TEST_DIR)/test_lru.cpp
	@$(MKDIR_P)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "🔁 Built LRU test: $(LRU_TEST)"

# ---------- TTL HEAP TEST ----------
$(TTL_TEST): $(DATA_DIR)/ttl_heap.cpp $(TEST_DIR)/test_heap.cpp
	@$(MKDIR_P)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "⏳ Built TTL Heap test: $(TTL_TEST)"

# ---------- INTEGRATION TEST ----------
$(INTEGRATION_TEST): $(DATA_SRC) $(TEST_DIR)/test_integration.cpp
	@$(MKDIR_P)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "🧠 Built Integration test: $(INTEGRATION_TEST)"

# =============================
# Run Targets
# =============================

run:
	@echo "🚀 Running main program..."
	@$(MAIN_TARGET)

test_hashmap: $(HASHMAP_TEST)
	@echo "🧪 Running HashMap test..."
	@$(HASHMAP_TEST)

test_lru: $(LRU_TEST)
	@echo "🧪 Running LRU test..."
	@$(LRU_TEST)

test_ttl: $(TTL_TEST)
	@echo "🧪 Running TTL Heap test..."
	@$(TTL_TEST)

test_integration: $(INTEGRATION_TEST)
	@echo "🧪 Running Integration test..."
	@$(INTEGRATION_TEST)

test_all: test_hashmap test_lru test_ttl test_integration

# =============================
# Cleanup
# =============================

clean:
	rm -rf $(BUILD_DIR)
	@echo "🧹 Cleaned build directory."

.PHONY: all clean run test_hashmap test_lru test_ttl test_integration test_all
