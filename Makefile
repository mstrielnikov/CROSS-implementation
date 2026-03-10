# CROSS Build System
# Supports Reference and Optimized implementations

CC ?= gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O3

# Include paths - order matters!
# Optimized needs its headers first to get SIMD functions
REF_DIR = Reference_Implementation
OPT_DIR = Optimized_Implementation
UNIFIED_DIR = Unified_Implementation

# Default parameters
CATEGORY ?= 1
VARIANT ?= RSDP
TARGET ?= BALANCED

# Security category (1, 3, 5), variant (RSDP/RSDPG), target (BALANCED/SPEED/SIG_SIZE)
CFLAGS += -DCATEGORY_$(CATEGORY)=1 -D$(VARIANT)=1 -D$(TARGET)=1

# Include paths - order matters for Optimized!
INCLUDES_REF = -I$(REF_DIR)/include -I$(UNIFIED_DIR)/include
INCLUDES_OPT = -I$(OPT_DIR)/include -I$(REF_DIR)/include -I$(UNIFIED_DIR)/include
INCLUDES_TEST = -I$(REF_DIR)/include -I$(UNIFIED_DIR)/include -IAdditional_Implementations/Benchmarking/include
INCLUDES_TEST_OPT = -I$(OPT_DIR)/include -I$(REF_DIR)/include -I$(UNIFIED_DIR)/include -IAdditional_Implementations/Benchmarking/include

# Source files
REF_SOURCES = \
	$(REF_DIR)/lib/CROSS.c \
	$(REF_DIR)/lib/sign.c \
	$(REF_DIR)/lib/merkle.c \
	$(REF_DIR)/lib/seedtree.c \
	$(REF_DIR)/lib/csprng_hash.c \
	$(REF_DIR)/lib/pack_unpack.c \
	$(REF_DIR)/lib/keccakf1600.c \
	$(REF_DIR)/lib/fips202.c

OPT_SOURCES = \
	$(OPT_DIR)/lib/CROSS.c \
	$(OPT_DIR)/lib/merkle.c \
	$(OPT_DIR)/lib/seedtree.c \
	$(OPT_DIR)/lib/fips202x4.c \
	$(OPT_DIR)/lib/KeccakP-1600-times4-SIMD256.c \
	$(REF_DIR)/lib/csprng_hash.c \
	$(REF_DIR)/lib/pack_unpack.c \
	$(REF_DIR)/lib/keccakf1600.c \
	$(REF_DIR)/lib/fips202.c \
	$(REF_DIR)/lib/sign.c

TEST_SOURCE = Additional_Implementations/Benchmarking/lib/CROSS_test.c

.PHONY: all ref opt test clean help

all: ref opt test

ref: CFLAGS_REF = $(CFLAGS) $(INCLUDES_REF)
ref: libcross_ref.a

libcross_ref.a: $(REF_SOURCES)
	@mkdir -p $(REF_DIR)/lib
	$(CC) $(CFLAGS_REF) -c $(REF_DIR)/lib/CROSS.c -o $(REF_DIR)/lib/CROSS.o
	$(CC) $(CFLAGS_REF) -c $(REF_DIR)/lib/sign.c -o $(REF_DIR)/lib/sign.o
	$(CC) $(CFLAGS_REF) -c $(REF_DIR)/lib/merkle.c -o $(REF_DIR)/lib/merkle.o
	$(CC) $(CFLAGS_REF) -c $(REF_DIR)/lib/seedtree.c -o $(REF_DIR)/lib/seedtree.o
	$(CC) $(CFLAGS_REF) -c $(REF_DIR)/lib/csprng_hash.c -o $(REF_DIR)/lib/csprng_hash.o
	$(CC) $(CFLAGS_REF) -c $(REF_DIR)/lib/pack_unpack.c -o $(REF_DIR)/lib/pack_unpack.o
	$(CC) $(CFLAGS_REF) -c $(REF_DIR)/lib/keccakf1600.c -o $(REF_DIR)/lib/keccakf1600.o
	$(CC) $(CFLAGS_REF) -c $(REF_DIR)/lib/fips202.c -o $(REF_DIR)/lib/fips202.o
	ar rcs $@ $(REF_DIR)/lib/CROSS.o $(REF_DIR)/lib/sign.o $(REF_DIR)/lib/merkle.o $(REF_DIR)/lib/seedtree.o $(REF_DIR)/lib/csprng_hash.o $(REF_DIR)/lib/pack_unpack.o $(REF_DIR)/lib/keccakf1600.o $(REF_DIR)/lib/fips202.o

opt: CFLAGS_OPT = $(CFLAGS) $(INCLUDES_OPT)
opt: libcross_opt.a

libcross_opt.a: $(OPT_SOURCES)
	@mkdir -p $(OPT_DIR)/lib
	$(CC) $(CFLAGS_OPT) -c $(OPT_DIR)/lib/CROSS.c -o $(OPT_DIR)/lib/CROSS.o
	$(CC) $(CFLAGS_OPT) -c $(OPT_DIR)/lib/merkle.c -o $(OPT_DIR)/lib/merkle.o
	$(CC) $(CFLAGS_OPT) -c $(OPT_DIR)/lib/seedtree.c -o $(OPT_DIR)/lib/seedtree.o
	$(CC) $(CFLAGS_OPT) -c $(OPT_DIR)/lib/fips202x4.c -o $(OPT_DIR)/lib/fips202x4.o
	$(CC) $(CFLAGS_OPT) -c $(OPT_DIR)/lib/KeccakP-1600-times4-SIMD256.c -o $(OPT_DIR)/lib/KeccakP-1600-times4-SIMD256.o
	$(CC) $(CFLAGS_OPT) -c $(REF_DIR)/lib/csprng_hash.c -o $(REF_DIR)/lib/csprng_hash.o
	$(CC) $(CFLAGS_OPT) -c $(REF_DIR)/lib/pack_unpack.c -o $(REF_DIR)/lib/pack_unpack.o
	$(CC) $(CFLAGS_OPT) -c $(REF_DIR)/lib/keccakf1600.c -o $(REF_DIR)/lib/keccakf1600.o
	$(CC) $(CFLAGS_OPT) -c $(REF_DIR)/lib/fips202.c -o $(REF_DIR)/lib/fips202.o
	$(CC) $(CFLAGS_OPT) -c $(REF_DIR)/lib/sign.c -o $(REF_DIR)/lib/sign.o
	ar rcs $@ $(OPT_DIR)/lib/CROSS.o $(OPT_DIR)/lib/merkle.o $(OPT_DIR)/lib/seedtree.o $(OPT_DIR)/lib/fips202x4.o $(OPT_DIR)/lib/KeccakP-1600-times4-SIMD256.o $(REF_DIR)/lib/csprng_hash.o $(REF_DIR)/lib/pack_unpack.o $(REF_DIR)/lib/keccakf1600.o $(REF_DIR)/lib/fips202.o $(REF_DIR)/lib/sign.o

test: CATEGORY=$(CATEGORY) VARIANT=$(VARIANT) TARGET=$(TARGET)
test:
	@if [ "$(IMPL)" = "opt" ]; then \
		echo "Building test with OPTIMIZED implementation..."; \
		$(CC) $(CFLAGS) $(INCLUDES_TEST_OPT) $(OPT_SOURCES) $(TEST_SOURCE) -o cross_test -lm; \
	else \
		echo "Building test with REFERENCE implementation..."; \
		$(CC) $(CFLAGS) $(INCLUDES_TEST) $(REF_SOURCES) $(TEST_SOURCE) -o cross_test -lm; \
	fi

clean:
	rm -f $(REF_DIR)/lib/*.o $(OPT_DIR)/lib/*.o libcross_ref.a libcross_opt.a cross_test

help:
	@echo "CROSS Build System"
	@echo ""
	@echo "Usage: make [target] [CATEGORY=1|3|5] [VARIANT=RSDP|RSDPG] [TARGET=BALANCED|SPEED|SIG_SIZE] [IMPL=ref|opt]"
	@echo ""
	@echo "Targets:"
	@echo "  ref           Build reference implementation library (libcross_ref.a)"
	@echo "  opt           Build optimized AVX2 implementation library (libcross_opt.a)"
	@echo "  test          Build test binary (cross_test)"
	@echo "  all           Build ref, opt, and test"
	@echo "  clean         Remove build artifacts"
	@echo ""
	@echo "Parameters:"
	@echo "  CATEGORY      Security category: 1, 3, or 5 (default: 1)"
	@echo "  VARIANT       Algorithm variant: RSDP or RSDPG (default: RSDP)"
	@echo "  TARGET        Optimization: BALANCED, SPEED, or SIG_SIZE (default: BALANCED)"
	@echo "  IMPL          Implementation for test: ref or opt (default: ref)"
	@echo ""
	@echo "Examples:"
	@echo "  make ref                              # Reference, Category 1, RSDP, BALANCED"
	@echo "  make ref CATEGORY=3 VARIANT=RSDPG    # Category 3, RSDPG variant"
	@echo "  make test IMPL=opt TARGET=SPEED      # Optimized test with SPEED target"
