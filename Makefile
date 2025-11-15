EXAMPLE_DIR  := examples
TEST_DIR     := tests
OUT_DIR      := build

COMMON_FLAGS := -Wall -Wextra -Werror -pedantic -Isrc -O0

ifndef NO_FSANITIZE
	COMMON_FLAGS += -g3 -fsanitize=address,leak,undefined
endif

CFLAGS       := $(COMMON_FLAGS) -std=c99 -Wstrict-prototypes
CXXFLAGS     := $(COMMON_FLAGS) -std=c++11

C_EXAMPLES   := $(patsubst %.c, $(OUT_DIR)/%, $(wildcard $(EXAMPLE_DIR)/*.c))
CXX_EXAMPLES := $(patsubst %.cc, $(OUT_DIR)/%, $(wildcard $(EXAMPLE_DIR)/*.cc))

C_TESTS      := $(patsubst %.c, $(OUT_DIR)/%, $(wildcard $(TEST_DIR)/*.c))
CXX_TESTS    := $(patsubst %.cc, $(OUT_DIR)/%, $(wildcard $(TEST_DIR)/*.cc))

.PHONY: all clean examples tests test
all: examples

clean:
	rm -rf $(OUT_DIR)

examples: $(C_EXAMPLES) $(CXX_EXAMPLES)

tests: test

test: $(C_TESTS) $(CXX_TESTS)
	FROM_MAKE=1 ./test.sh $(TEST_DIR) $(OUT_DIR)/$(TEST_DIR)

$(OUT_DIR)/%: %.c src/args.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

$(OUT_DIR)/%: %.cc src/args.h
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $< -o $@
