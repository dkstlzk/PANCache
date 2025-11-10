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
# SANITIZE := -fsanitize=address -fno-omit-frame-pointer
CXXFLAGS  := $(CXXSTD) $(CXXWARN) $(CXXOPT) $(INCLUDES) $(DEPFLAGS) $(SANITIZE)

# ===============================
# 📁 Project Layout
# ===============================
SRC_DIR    := src
DATA_DIR   := $(SRC_DIR)/data
TEST_DIR   := tests
BUILD_DIR  := build
OBJ_DIR    := $(BUILD_DIR)/obj

# ===============================
# 🧱 Source Files
# ===============================
MAIN_SRC   := $(SRC_DIR)/main.cpp
DATA_SRC := $(DATA_DIR)/hashmap.cpp $(DATA_DIR)/lru.cpp $(DATA_DIR)/ttl_heap.cpp $(DATA_DIR)/graph.cpp
DATA_SRC := $(DATA_DIR)/hashmap.cpp $(DATA_DIR)/lru.cpp $(DATA_DIR)/ttl_heap.cpp $(DATA_DIR)/graph.cpp $(DATA_DIR)/cache_engine.cpp

TEST_HASHMAP_SRC := $(TEST_DIR)/test_hashmap.cpp
TEST_LRU_SRC     := $(TEST_DIR)/test_lru.cpp
TEST_TTL_SRC     := $(TEST_DIR)/test_heap.cpp
TEST_INT_SRC     := $(TEST_DIR)/test_integration.cpp
TEST_SKIPLIST_SRC := $(TEST_DIR)/test_skiplist.cpp
TEST_GRAPH_SRC := $(TEST_DIR)/test_graph.cpp
TEST_CACHE_SRC := $(TEST_DIR)/test_cache.cpp

# ===============================
# 🏗️ Output Binaries
# ===============================
MAIN_TARGET  := $(BUILD_DIR)/pancache_main
HASHMAP_TEST := $(BUILD_DIR)/test_hashmap
LRU_TEST     := $(BUILD_DIR)/test_lru
TTL_TEST     := $(BUILD_DIR)/test_ttl
INTEG_TEST   := $(BUILD_DIR)/test_integration
SKIPLIST_TEST := $(BUILD_DIR)/test_skiplist
GRAPH_TEST     := $(BUILD_DIR)/test_graph
CACHE_TEST     := $(BUILD_DIR)/test_cache

# ===============================
# 🧩 Object Files
# ===============================
MAIN_OBJ   := $(OBJ_DIR)/main.o
DATA_OBJ   := $(patsubst $(DATA_DIR)/%.cpp,$(OBJ_DIR)/data_%.o,$(DATA_SRC))
THASH_OBJ  := $(OBJ_DIR)/t_hashmap.o
TLRU_OBJ   := $(OBJ_DIR)/t_lru.o
TTTL_OBJ   := $(OBJ_DIR)/t_ttl.o
TINT_OBJ   := $(OBJ_DIR)/t_integration.o
TOBJ_GRAPH     := $(OBJ_DIR)/t_graph.o

.DEFAULT_GOAL := help

# ===============================
# 🎯 Meta Targets
# ===============================
all: $(MAIN_TARGET) $(HASHMAP_TEST) $(LRU_TEST) $(TTL_TEST) $(INTEG_TEST) $(GRAPH_TEST) $(CACHE_TEST)


# ===============================
# 🧩 Build Rules
# ===============================
$(MAIN_TARGET): $(MAIN_OBJ) $(DATA_OBJ) | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built main: $@"

$(HASHMAP_TEST): $(THASH_OBJ) $(OBJ_DIR)/data_hashmap.o | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built HashMap test: $@"

$(LRU_TEST): $(TLRU_OBJ) $(OBJ_DIR)/data_hashmap.o $(OBJ_DIR)/data_lru.o | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built LRU test: $@"

$(TTL_TEST): $(TTTL_OBJ) $(OBJ_DIR)/data_ttl_heap.o | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built TTL Heap test: $@"

$(INTEG_TEST): $(TINT_OBJ) $(DATA_OBJ) | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built Integration test: $@"

$(SKIPLIST_TEST): $(TEST_SKIPLIST_SRC) | $(BUILD_DIR)
	@echo "Compiling SkipList test..."
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built SkipList test: $@"

$(GRAPH_TEST): $(TOBJ_GRAPH) $(OBJ_DIR)/data_graph.o | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built Graph test: $@"

$(CACHE_TEST): $(OBJ_DIR)/t_cache.o $(OBJ_DIR)/data_cache_engine.o $(OBJ_DIR)/data_hashmap.o $(OBJ_DIR)/data_lru.o $(OBJ_DIR)/data_ttl_heap.o $(OBJ_DIR)/data_graph.o | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built CacheEngine test: $@"


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

$(OBJ_DIR)/t_hashmap.o: $(TEST_HASHMAP_SRC) | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/t_lru.o: $(TEST_LRU_SRC) | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/t_ttl.o: $(TEST_TTL_SRC) | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/t_integration.o: $(TEST_INT_SRC) | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/t_graph.o: $(TEST_GRAPH_SRC) | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/t_cache.o: $(TEST_CACHE_SRC) | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@


# ===============================
# 🧾 Dependencies
# ===============================
-include $(wildcard $(OBJ_DIR)/*.d)

# ===============================
# ▶️ Run Commands
# ===============================
run_main: $(MAIN_TARGET)
	@echo "Running PANCache main..."
	@$(MAIN_TARGET)

test_hashmap: $(HASHMAP_TEST)
	@echo "Running HashMap test..."
	@$(HASHMAP_TEST)

test_lru: $(LRU_TEST)
	@echo "Running LRU test..."
	@$(LRU_TEST)

test_ttl: $(TTL_TEST)
	@echo "Running TTL Heap test..."
	@$(TTL_TEST)

test_integration: $(INTEG_TEST)
	@echo "Running Integration test..."
	@$(INTEG_TEST)

test_skiplist: $(SKIPLIST_TEST)
	@echo "Running SkipList test..."
	@$(SKIPLIST_TEST)

test_graph: $(GRAPH_TEST)
	@echo "Running Graph test..."
	@$(GRAPH_TEST)

test_cache: $(CACHE_TEST)
	@echo "Running CacheEngine test..."
	@$(CACHE_TEST)

test_all: test_hashmap test_lru test_ttl test_graph test_skiplist test_integration test_cache


# ===============================
# 🧹 Clean + Help
# ===============================
clean:
	@$(RMDIR_CMD)
	@echo "Cleaned build directory."

help:
	@echo "PANCache — Make targets:"
	@echo "  mingw32-make all              # Build main + all tests"
	@echo "  mingw32-make run_main         # Run main program"
	@echo "  mingw32-make test_hashmap     # Build+run HashMap test"
	@echo "  mingw32-make test_lru         # Build+run LRU test"
	@echo "  mingw32-make test_ttl         # Build+run TTL Heap test"
	@echo "  mingw32-make test_integration # Build+run integration test"
	@echo "  mingw32-make clean            # Remove build artifacts"
	@echo "  (set SANITIZE in Makefile to enable AddressSanitizer)"

.PHONY: all clean help run_main test_hashmap test_lru test_ttl test_integration test_all
