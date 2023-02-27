# ==================================================
# Parameters
# ==================================================

# Path to the LLVM installation for finding `clang`.
# Empty if use `clang` in PATH.
LLVM_HOME ?=

# System root of riscv64 C library.
SYS_ROOT ?= /usr/riscv64-linux-gnu

# Path to the GCC toolchain.
GCC_TOOLCHAIN ?= /usr

# ==================================================
# Directories
# ==================================================

TOP_DIR := $(shell pwd)
SRC_DIR := $(TOP_DIR)/src
BENCH_DIR := $(SRC_DIR)/bench
UTILS_DIR := $(SRC_DIR)/utils

BUILD_DIR := $(TOP_DIR)/build

# ==================================================
# Cross Compiler
# ==================================================

ifeq ($(LLVM_HOME),)
CROSS_CLANG := clang
else
CROSS_CLANG := $(LLVM_HOME)/bin/clang
endif

CFLAGS := --target=riscv64-unknown-linux-elf -march=rv64gc_xsmx
CFLAGS += --sysroot=$(SYS_ROOT) --gcc-toolchain=$(GCC_TOOLCHAIN)
CFLAGS += -I $(UTILS_DIR) -I $(BENCH_DIR) -O3 -fuse-ld=lld -static

# ==================================================
# Benchmarks
# ==================================================

BENCH := $(shell find $(BENCH_DIR) -name '*.c')
BENCH_EXE := $(patsubst $(BENCH_DIR)/%.c, $(BUILD_DIR)/%, $(BENCH))

.PHONY: clean

bench: $(BENCH_EXE)

clean:
	-rm -rf $(BUILD_DIR)

$(BUILD_DIR)/%: $(BENCH_DIR)/%.c $(UTILS_DIR)/smxintrin.h
	mkdir -p $(dir $@)
	$(CROSS_CLANG) $(CFLAGS) $< -o $@
