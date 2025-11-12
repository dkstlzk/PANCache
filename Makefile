# ===========================================
# 🧠 PANCache Unified Build System
# ===========================================

# -------- Cross-Platform Directory Handling --------
ifeq ($(OS),Windows_NT)
    MKDIR_CMD = if not exist "$(subst /,\\,$(1))" mkdir "$(subst /,\\,$(1))"
    RMDIR_CMD = if exist "$(subst /,\\,$(BUILD_DIR))" rmdir /s /q "$(subst /,\\,$(BUILD_DIR))"
else
    MKDIR_CMD = mkdir -p $(1)
    RMDIR_CMD = rm -rf $(BUILD_DIR)
endif

# -------- Toolchain Config --------
CXX       := g++
CXXSTD    := -std=c++17
CXXWARN   := -Wall -Wextra
CXXOPT    := -O2
INCLUDES  := -Iinclude
DEPFLAGS  := -MMD -MP
CXXFLAGS  := $(CXXSTD) $(CXXWARN) $(CXXOPT) $(INCLUDES) $(DEPFLAGS)

# -------- Project Layout --------
SRC_DIR       := src
DATA_DIR      := $(SRC_DIR)/data
DEPEND_DIR    := $(SRC_DIR)/depend
ANALYTICS_DIR := $(SRC_DIR)/analytics
CLI_DIR       := $(SRC_DIR)/cli
# UTILS_DIR   := $(SRC_DIR)/utils
TEST_DIR      := tests
BUILD_DIR     := build
OBJ_DIR       := $(BUILD_DIR)/obj

# ===========================================
# 🧩 Source Files
# ===========================================
MAIN_SRC := $(SRC_DIR)/main.cpp

DATA_SRC := \
    $(DATA_DIR)/hashmap.cpp \
    $(DATA_DIR)/lru.cpp \
    $(DATA_DIR)/ttl_heap.cpp \
    $(DATA_DIR)/cache_engine.cpp


DEPEND_SRC    := $(DEPEND_DIR)/graph.cpp
ANALYTICS_SRC := $(ANALYTICS_DIR)/topk.cpp
CLI_SRC       := $(CLI_DIR)/command_parser.cpp
# UTILS_SRC   := $(UTILS_DIR)/time_utils.cpp

# ===========================================
# 🧪 Test Source Files
# ===========================================
TEST_HASHMAP_SRC  := $(TEST_DIR)/test_hashmap.cpp
TEST_LRU_SRC      := $(TEST_DIR)/test_lru.cpp
TEST_TTL_SRC      := $(TEST_DIR)/test_heap.cpp
TEST_SKIPLIST_SRC := $(TEST_DIR)/test_skiplist.cpp
TEST_GRAPH_SRC    := $(TEST_DIR)/test_graph.cpp
TEST_CACHE_SRC    := $(TEST_DIR)/test_cache.cpp
TEST_INTEG_SRC    := $(TEST_DIR)/test_integration.cpp
TEST_CLI_SRC      := $(TEST_DIR)/test_cli.cpp

# ===========================================
# 🎯 Binaries
# ===========================================
MAIN_TARGET   := $(BUILD_DIR)/pancache_main
HASHMAP_TEST  := $(BUILD_DIR)/test_hashmap
LRU_TEST      := $(BUILD_DIR)/test_lru
TTL_TEST      := $(BUILD_DIR)/test_ttl
SKIPLIST_TEST := $(BUILD_DIR)/test_skiplist
GRAPH_TEST    := $(BUILD_DIR)/test_graph
CACHE_TEST    := $(BUILD_DIR)/test_cache
INTEG_TEST    := $(BUILD_DIR)/test_integration
CLI_TEST      := $(BUILD_DIR)/test_cli

# ===========================================
# 🧱 Objects
# ===========================================
MAIN_OBJ      := $(OBJ_DIR)/main.o
DATA_OBJ      := $(patsubst $(DATA_DIR)/%.cpp,$(OBJ_DIR)/data_%.o,$(DATA_SRC))
DEPEND_OBJ    := $(patsubst $(DEPEND_DIR)/%.cpp,$(OBJ_DIR)/depend_%.o,$(DEPEND_SRC))
ANALYTICS_OBJ := $(patsubst $(ANALYTICS_DIR)/%.cpp,$(OBJ_DIR)/analytics_%.o,$(ANALYTICS_SRC))
CLI_OBJ       := $(patsubst $(CLI_DIR)/%.cpp,$(OBJ_DIR)/cli_%.o,$(CLI_SRC))
# UTILS_OBJ   := $(patsubst $(UTILS_DIR)/%.cpp,$(OBJ_DIR)/utils_%.o,$(UTILS_SRC))

ALL_OBJ := $(MAIN_OBJ) $(DATA_OBJ) $(DEPEND_OBJ) $(ANALYTICS_OBJ) $(CLI_OBJ)

# ===========================================
# 🧰 Ensure Build Directories Exist
# ===========================================
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# ===========================================
# 🧱 Build Targets
# ===========================================
all: $(MAIN_TARGET) $(HASHMAP_TEST) $(LRU_TEST) $(TTL_TEST) \
     $(SKIPLIST_TEST) $(GRAPH_TEST) $(CACHE_TEST) $(INTEG_TEST) $(CLI_TEST)
.DEFAULT_GOAL := help

# -------- Main Build --------
$(MAIN_TARGET): $(ALL_OBJ) | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "✅ Built main executable: $@"

# ===========================================
# 🧪 Test Targets
# ===========================================
# -------- Tests Build --------
$(HASHMAP_TEST): $(TEST_HASHMAP_SRC) $(DATA_OBJ) $(DEPEND_OBJ) | $(BUILD_DIR)
	@echo "🧩 Building HashMap test..."
	$(CXX) $(CXXFLAGS) $^ -o $@

$(LRU_TEST): $(TEST_LRU_SRC) $(DATA_OBJ) $(DEPEND_OBJ) | $(BUILD_DIR)
	@echo "🔁 Building LRU test..."
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TTL_TEST): $(TEST_TTL_SRC) $(DATA_OBJ) $(DEPEND_OBJ) | $(BUILD_DIR)
	@echo "⏳ Building TTL test..."
	$(CXX) $(CXXFLAGS) $^ -o $@

$(SKIPLIST_TEST): $(TEST_SKIPLIST_SRC) $(DATA_OBJ) $(DEPEND_OBJ) | $(BUILD_DIR)
	@echo "📊 Building SkipList test..."
	$(CXX) $(CXXFLAGS) $^ -o $@

$(GRAPH_TEST): $(TEST_GRAPH_SRC) $(DEPEND_OBJ) | $(BUILD_DIR)
	@echo "🔗 Building Graph test..."
	$(CXX) $(CXXFLAGS) $^ -o $@

$(CACHE_TEST): $(TEST_CACHE_SRC) $(DATA_OBJ) $(DEPEND_OBJ) | $(BUILD_DIR)
	@echo "🧮 Building CacheEngine test..."
	$(CXX) $(CXXFLAGS) $^ -o $@

$(INTEG_TEST): $(TEST_INTEG_SRC) $(DATA_OBJ) $(DEPEND_OBJ) $(CLI_OBJ) $(ANALYTICS_OBJ) | $(BUILD_DIR)
	@echo "🧠 Building Integration test..."
	$(CXX) $(CXXFLAGS) $^ -o $@

$(CLI_TEST): $(TEST_CLI_SRC) $(CLI_OBJ) $(DATA_OBJ) $(DEPEND_OBJ) $(ANALYTICS_OBJ) | $(BUILD_DIR)
	@echo "💻 Building CLI test..."
	$(CXX) $(CXXFLAGS) $^ -o $@

# ===========================================
# 🧰 Object Compilation Rules
# ===========================================
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp | $(OBJ_DIR)
	@echo "Compiling main.cpp"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/data_%.o: $(DATA_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/depend_%.o: $(DEPEND_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/analytics_%.o: $(ANALYTICS_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/cli_%.o: $(CLI_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ===========================================
# ▶️ Run + Test Commands
# ===========================================
run_main: $(MAIN_TARGET)
	@echo "🚀 Running PANCache main..."
	@$(MAIN_TARGET)

test_hashmap: $(HASHMAP_TEST); @$(HASHMAP_TEST)
test_lru:     $(LRU_TEST);     @$(LRU_TEST)
test_ttl:     $(TTL_TEST);     @$(TTL_TEST)
test_skiplist:$(SKIPLIST_TEST);@$(SKIPLIST_TEST)
test_graph:   $(GRAPH_TEST);   @$(GRAPH_TEST)
test_cache:   $(CACHE_TEST);   @$(CACHE_TEST)
test_cli:     $(CLI_TEST);     @$(CLI_TEST)
test_integration: $(INTEG_TEST); @$(INTEG_TEST)

test_all: test_hashmap test_lru test_ttl test_skiplist \
          test_graph test_cache test_cli test_integration
	@echo "🎯 All PANCache tests passed successfully!"

# ===========================================
# 🧹 Clean + Help
# ===========================================
clean:
	@$(RMDIR_CMD)
	@echo "🧹 Cleaned build directory."

help:
	@echo "PANCache — Cross-Platform Build System"
	@echo ""
	@echo "Commands:"
	@echo "  make all              # Build all modules"
	@echo "  make run_main         # Run the main executable"
	@echo "  make test_<module>    # Run specific module test"
	@echo "  make test_all         # Build & run all tests"
	@echo "  make clean            # Clean build directory"
	@echo "  make help             # Show this help"

.PHONY: all clean help run_main \
        test_hashmap test_lru test_ttl test_skiplist \
        test_graph test_cache test_cli test_integration test_all
