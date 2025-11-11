# ===============================
# 🧩 Cross-Platform Directory Handling
# ===============================
ifeq ($(OS),Windows_NT)
    # Windows (MinGW, Git Bash, PowerShell)
    MKDIR_CMD = if not exist "$(subst /,\\,$(1))" mkdir "$(subst /,\\,$(1))"
    RMDIR_CMD = if exist "$(subst /,\\,$(BUILD_DIR))" rmdir /s /q "$(subst /,\\,$(BUILD_DIR))"
else
    # Linux/macOS
    MKDIR_CMD = mkdir -p $(1)
    RMDIR_CMD = rm -rf $(BUILD_DIR)
endif

# ===============================
# ⚙️ Toolchain Configuration
# ===============================
CXXSTD    := -std=c++17
CXXWARN   := -Wall -Wextra
CXXOPT    := -O2
INCLUDES  := -Iinclude
DEPFLAGS  := -MMD -MP
CXXFLAGS  := $(CXXSTD) $(CXXWARN) $(CXXOPT) $(INCLUDES) $(DEPFLAGS)

# ===============================
# 📁 Project Layout
# ===============================
SRC_DIR    := src
DATA_DIR   := $(SRC_DIR)/data
DEPEND_DIR := $(SRC_DIR)/depend      
TEST_DIR   := tests
BUILD_DIR  := build
OBJ_DIR    := $(BUILD_DIR)/obj

# ===============================
# 🧱 Source Files
# ===============================
MAIN_SRC   := $(SRC_DIR)/main.cpp
DATA_SRC   := $(DATA_DIR)/hashmap.cpp $(DATA_DIR)/lru.cpp $(DATA_DIR)/ttl_heap.cpp


TEST_HASHMAP_SRC  := $(TEST_DIR)/test_hashmap.cpp
TEST_LRU_SRC      := $(TEST_DIR)/test_lru.cpp
TEST_TTL_SRC      := $(TEST_DIR)/test_heap.cpp
TEST_INT_SRC      := $(TEST_DIR)/test_integration.cpp
TEST_SKIPLIST_SRC := $(TEST_DIR)/test_skiplist.cpp
TEST_GRAPH_SRC    := $(TEST_DIR)/test_graph.cpp   # ✅ Add graph test
TEST_CACHE_SRC    := $(TEST_DIR)/test_cache.cpp

# ===============================
# 🏗️ Output Binaries
# ===============================
MAIN_TARGET   := $(BUILD_DIR)/pancache_main
HASHMAP_TEST  := $(BUILD_DIR)/test_hashmap
LRU_TEST      := $(BUILD_DIR)/test_lru
TTL_TEST      := $(BUILD_DIR)/test_ttl
INTEG_TEST    := $(BUILD_DIR)/test_integration
SKIPLIST_TEST := $(BUILD_DIR)/test_skiplist
GRAPH_TEST    := $(BUILD_DIR)/test_graph    # ✅ Add graph test binary
CACHE_TEST    := $(BUILD_DIR)/test_cache

# ===============================
# 🧩 Object Files
# ===============================
MAIN_OBJ      := $(OBJ_DIR)/main.o
DATA_OBJ      := $(patsubst $(DATA_DIR)/%.cpp,$(OBJ_DIR)/data_%.o,$(DATA_SRC))
TOBJ_GRAPH    := $(OBJ_DIR)/t_graph.o

# ===============================
# 🎯 Meta Targets
# ===============================
all: $(MAIN_TARGET) $(HASHMAP_TEST) $(LRU_TEST) $(TTL_TEST) $(INTEG_TEST) $(GRAPH_TEST) $(SKIPLIST_TEST) $(CACHE_TEST)

.DEFAULT_GOAL := help

# ===============================
# 🧩 Build Rules
# ===============================
$(MAIN_TARGET): $(MAIN_OBJ) $(DATA_OBJ) $(DEPEND_SRC) | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "✅ Built main: $@"

$(HASHMAP_TEST): $(OBJ_DIR)/t_hashmap.o $(OBJ_DIR)/data_hashmap.o | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "🧩 Built HashMap test: $@"

$(LRU_TEST): $(OBJ_DIR)/t_lru.o $(OBJ_DIR)/data_hashmap.o $(OBJ_DIR)/data_lru.o | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "🔁 Built LRU test: $@"

$(TTL_TEST): $(OBJ_DIR)/t_ttl.o $(OBJ_DIR)/data_ttl_heap.o | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "⏳ Built TTL Heap test: $@"

$(GRAPH_TEST): $(TEST_GRAPH_SRC) | $(BUILD_DIR)

	@$(call MKDIR_CMD,$(BUILD_DIR))
	@echo "🔗 Building Graph Dependency test..."
	$(CXX) $(CXXFLAGS) $(DEPEND_SRC) $(TEST_GRAPH_SRC) -o $(GRAPH_TEST)
	@echo "✅ Built Graph Dependency test: $(GRAPH_TEST)"


$(INTEG_TEST): $(OBJ_DIR)/t_integration.o $(DATA_OBJ) $(DEPEND_SRC) | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "🧠 Built Integration test: $@"

$(SKIPLIST_TEST): $(TEST_SKIPLIST_SRC) | $(BUILD_DIR)
	@echo "Compiling SkipList test..."
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "📊 Built SkipList test: $@"

$(CACHE_TEST): $(OBJ_DIR)/t_cache.o $(OBJ_DIR)/data_cache_engine.o $(DATA_OBJ) $(DEPEND_SRC) | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "🧮 Built CacheEngine test: $@"

# ===============================
# 🧰 Directory Handling
# ===============================
$(BUILD_DIR) $(OBJ_DIR):
	@$(call MKDIR_CMD,$@)

# ===============================
# 🧱 Compilation Rules
# ===============================
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/data_%.o: $(DATA_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/t_%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ===============================
# ▶️ Run Commands
# ===============================
run_main: $(MAIN_TARGET)
	@echo "🚀 Running PANCache main..."
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

test_graph: $(GRAPH_TEST)
	@echo "🧪 Running Graph Dependency test..."
	@$(GRAPH_TEST)

test_integration: $(INTEG_TEST)
	@echo "🧪 Running Integration test..."
	@$(INTEG_TEST)

test_skiplist: $(SKIPLIST_TEST)
	@echo "🧪 Running SkipList test..."
	@$(SKIPLIST_TEST)

test_cache: $(CACHE_TEST)
	@echo "🧪 Running CacheEngine test..."
	@$(CACHE_TEST)

test_all: test_hashmap test_lru test_ttl test_graph test_skiplist test_integration test_cache

# ===============================
# 🧹 Clean + Help
# ===============================
clean:
	@$(RMDIR_CMD)
	@echo "🧹 Cleaned build directory."

help:
	@echo "PANCache — Make targets:"
	@echo "  make all              # Build all components"
	@echo "  make run_main         # Run main executable"
	@echo "  make test_hashmap     # Build + run HashMap test"
	@echo "  make test_graph       # Build + run Graph Dependency test"
	@echo "  make test_all         # Run all tests"
	@echo "  make clean            # Clean build directory"

.PHONY: all clean help run_main test_hashmap test_lru test_ttl test_graph test_skiplist test_integration test_cache test_all
