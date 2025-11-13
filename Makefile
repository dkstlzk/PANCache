# ===========================================
# 🧠 PANCache Unified Build System (Cross-Platform)
# ===========================================

# -------- Platform Handling --------
ifeq ($(OS),Windows_NT)
    MKDIR = if not exist "$(subst /,\\,$(1))" mkdir "$(subst /,\\,$(1))"
    RMDIR = if exist "$(subst /,\\,$(1))" rmdir /s /q "$(subst /,\\,$(1))"
    EXE := .exe
else
    MKDIR = mkdir -p $(1)
    RMDIR = rm -rf $(1)
    EXE :=
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
UTIL_DIR      := $(SRC_DIR)/utils
TEST_DIR      := tests
BUILD_DIR     := build
OBJ_DIR       := $(BUILD_DIR)/obj

# ===========================================
# 🧩 Source Files
# ===========================================
MAIN_SRC := $(SRC_DIR)/main.cpp
DATA_SRC := $(DATA_DIR)/hashmap.cpp $(DATA_DIR)/lru.cpp $(DATA_DIR)/ttl_heap.cpp $(DATA_DIR)/cache_engine.cpp
DEPEND_SRC := $(DEPEND_DIR)/graph.cpp
ANALYTICS_SRC := $(ANALYTICS_DIR)/topk.cpp
CLI_SRC := $(CLI_DIR)/command_parser.cpp
UTIL_SRC := $(UTIL_DIR)/logger.cpp

# ===========================================
# 🧪 Test Sources
# ===========================================
TESTS := \
    test_hashmap test_lru test_heap test_skiplist test_graph \
    test_cache test_integration test_cli test_logger

# ===========================================
# 🧱 Build Targets
# ===========================================
MAIN_TARGET := $(BUILD_DIR)/pancache_main$(EXE)
TEST_TARGETS := $(patsubst %, $(BUILD_DIR)/%$(EXE), $(TESTS))

DATA_OBJ := $(patsubst $(DATA_DIR)/%.cpp,$(OBJ_DIR)/data_%.o,$(DATA_SRC))
DEPEND_OBJ := $(patsubst $(DEPEND_DIR)/%.cpp,$(OBJ_DIR)/depend_%.o,$(DEPEND_SRC))
ANALYTICS_OBJ := $(patsubst $(ANALYTICS_DIR)/%.cpp,$(OBJ_DIR)/analytics_%.o,$(ANALYTICS_SRC))
CLI_OBJ := $(patsubst $(CLI_DIR)/%.cpp,$(OBJ_DIR)/cli_%.o,$(CLI_SRC))
UTIL_OBJ := $(patsubst $(UTIL_DIR)/%.cpp,$(OBJ_DIR)/utils_%.o,$(UTIL_SRC))
MAIN_OBJ := $(OBJ_DIR)/main.o
ALL_OBJ := $(MAIN_OBJ) $(DATA_OBJ) $(DEPEND_OBJ) $(ANALYTICS_OBJ) $(CLI_OBJ) $(UTIL_OBJ)

# ===========================================
# 🧰 Directory Setup
# ===========================================
$(BUILD_DIR):
	@$(call MKDIR,$(BUILD_DIR))

$(OBJ_DIR):
	@$(call MKDIR,$(OBJ_DIR))

# ===========================================
# 🧱 Core Build
# ===========================================
all: $(MAIN_TARGET)
.DEFAULT_GOAL := help

$(MAIN_TARGET): $(ALL_OBJ) | $(BUILD_DIR)
	@echo "🚀 Building PANCache Core..."
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "✅ Built main executable: $@"

# ===========================================
# 🧩 Object Compilation
# ===========================================
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<"
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

$(OBJ_DIR)/utils_%.o: $(UTIL_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ===========================================
# 🧪 Tests
# ===========================================
# 👇 Fixed line: filter out main.o to avoid duplicate main()
$(BUILD_DIR)/%$(EXE): $(TEST_DIR)/%.cpp $(filter-out $(MAIN_OBJ),$(ALL_OBJ)) | $(BUILD_DIR)
	@echo "🧩 Building test: $@"
	$(CXX) $(CXXFLAGS) $^ -o $@

test_%: $(BUILD_DIR)/%$(EXE)
	@echo "Running test: $<"
	@$(BUILD_DIR)/$*$(EXE)

test_all: $(TEST_TARGETS)
	@echo "🎯 Running all tests..."
	@for t in $(TESTS); do \
		echo "👉 Running $$t..."; \
		$(BUILD_DIR)/$$t$(EXE) || exit 1; \
	done
	@echo "✅ All tests passed!"

# ===========================================
# 🧹 Clean + Help
# ===========================================
clean:
	@echo "🧹 Cleaning build directory..."
	@$(call RMDIR,$(BUILD_DIR))
	@echo "✅ Clean complete."

help:
	@echo ""
	@echo "🧠 PANCache Build System — Commands"
	@echo "----------------------------------"
	@echo "make all              → Build main executable"
	@echo "make test_all         → Build + run all tests"
	@echo "make test_<name>      → Build + run individual test"
	@echo "make clean            → Remove build directory"
	@echo "make help             → Show this help"
	@echo ""

.PHONY: all clean help test_all $(patsubst %,test_%,$(TESTS))
