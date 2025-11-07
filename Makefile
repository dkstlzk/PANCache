# toolchain
CXXSTD    := -std=c++17
CXXWARN   := -Wall -Wextra
CXXOPT    := -O2
INCLUDES  := -Iinclude
DEPFLAGS  := -MMD -MP              # auto-generate header dependencies
# SANITIZE := -fsanitize=address -fno-omit-frame-pointer  # <- enable while debugging
CXXFLAGS  := $(CXXSTD) $(CXXWARN) $(CXXOPT) $(INCLUDES) $(DEPFLAGS) $(SANITIZE)

# layout
SRC_DIR    := src
DATA_DIR   := $(SRC_DIR)/data
TEST_DIR   := tests
BUILD_DIR  := build
OBJ_DIR    := $(BUILD_DIR)/obj

# sources
MAIN_SRC   := $(SRC_DIR)/main.cpp
DATA_SRC   := $(DATA_DIR)/hashmap.cpp $(DATA_DIR)/lru.cpp $(DATA_DIR)/ttl_heap.cpp 
TEST_HASHMAP_SRC := $(TEST_DIR)/test_hashmap.cpp
TEST_LRU_SRC     := $(TEST_DIR)/test_lru.cpp
TEST_TTL_SRC     := $(TEST_DIR)/test_heap.cpp
TEST_INT_SRC     := $(TEST_DIR)/test_integration.cpp

# binaries
MAIN_TARGET  := $(BUILD_DIR)/pancache_main
HASHMAP_TEST := $(BUILD_DIR)/test_hashmap
LRU_TEST     := $(BUILD_DIR)/test_lru
TTL_TEST     := $(BUILD_DIR)/test_ttl
INTEG_TEST   := $(BUILD_DIR)/test_integration
SKIPLIST_TEST := $(BUILD_DIR)/test_skiplist


# objects
MAIN_OBJ   := $(OBJ_DIR)/main.o
DATA_OBJ   := $(patsubst $(DATA_DIR)/%.cpp,$(OBJ_DIR)/data_%.o,$(DATA_SRC))
THASH_OBJ  := $(OBJ_DIR)/t_hashmap.o
TLRU_OBJ   := $(OBJ_DIR)/t_lru.o
TTTL_OBJ   := $(OBJ_DIR)/t_ttl.o
TINT_OBJ   := $(OBJ_DIR)/t_integration.o

.DEFAULT_GOAL := help

# meta targets
all: $(MAIN_TARGET) $(HASHMAP_TEST) $(LRU_TEST) $(TTL_TEST) $(INTEG_TEST)

# main binary
$(MAIN_TARGET): $(MAIN_OBJ) $(DATA_OBJ) | $(BUILD_DIR)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built main: $@"

# tests
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

$(SKIPLIST_TEST): $(TEST_DIR)/test_skiplist.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built SkipList test: $(SKIPLIST_TEST)"


# COMPILE RULES

# ensure directories exist
$(BUILD_DIR) $(OBJ_DIR):
	@mkdir -p $@

# pattern rule: compile .cpp -> .o with depfile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# data objects
$(OBJ_DIR)/data_%.o: $(DATA_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# test objects
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

# include auto-generated dependency files (safe if missing)
-include $(wildcard $(OBJ_DIR)/*.d)

run_main: $(MAIN_TARGET)
	@echo "Running PANCache build verification...""
	@$(MAIN_TARGET)

test_hashmap: $(HASHMAP_TEST)
	@echo "Running HashMap test…"
	@$(HASHMAP_TEST)

test_lru: $(LRU_TEST)
	@echo "Running LRU test…"
	@$(LRU_TEST)

test_ttl: $(TTL_TEST)
	@echo "Running TTL Heap test…"
	@$(TTL_TEST)

test_integration: $(INTEG_TEST)
	@echo "Running Integration test…"
	@$(INTEG_TEST)

test_skiplist: $(SKIPLIST_TEST)
	@echo "Running SkipList test..."
	@$(SKIPLIST_TEST)

test_all: test_hashmap test_lru test_ttl test_skiplist test_integration

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory."

help:
	@echo "PANCache — Make targets:"
	@echo "  make all              # Build main + all tests"
	@echo "  make $(MAIN_TARGET)   # Build main only"
	@echo "  make run_hashmap      # Run main (HashMap demo)"
	@echo "  make test_hashmap     # Build+run HashMap test"
	@echo "  make test_lru         # Build+run LRU test"
	@echo "  make test_ttl         # Build+run TTL Heap test"
	@echo "  make test_integration # Build+run integration test"
	@echo "  make clean            # Remove build artifacts"
	@echo "  (set SANITIZE in Makefile to enable ASan)"

.PHONY: all clean help run_hashmap test_hashmap test_lru test_ttl test_integration test_all
