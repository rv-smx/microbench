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

# Path to libSMXTransforms.so.
SMX_TRANS ?=

# ==================================================
# Directories
# ==================================================

TOP_DIR := $(shell pwd)
SRC_DIR := $(TOP_DIR)/src
UTILS_DIR := $(SRC_DIR)/utils

BUILD_DIR := $(TOP_DIR)/build

# ==================================================
# Cross Compiler Toolchain
# ==================================================

ifeq ($(LLVM_HOME),)
CROSS_CLANG := clang
CROSS_OPT := opt
CROSS_LLC := llc
else
CROSS_CLANG := $(LLVM_HOME)/bin/clang
CROSS_OPT := $(LLVM_HOME)/bin/opt
CROSS_LLC := $(LLVM_HOME)/bin/llc
endif

CFLAGS := --target=riscv64-unknown-linux-elf -march=rv64gc_xsmx
CFLAGS += --sysroot=$(SYS_ROOT) --gcc-toolchain=$(GCC_TOOLCHAIN)
CFLAGS += -I $(SRC_DIR) -O3 -Wall -Werror

ifeq ($(SMX_TRANS),)
$(error "SMX transform library path is not specified!")
endif

OPTFLAGS := -load-pass-plugin=$(SMX_TRANS) -S
OPTFLAGS += -passes="instnamer,loop-simplify,loop-streamization"

LLCFLAGS := -O3 -filetype=obj

LDFLAGS := $(CFLAGS) -fuse-ld=lld -static

# ==================================================
# Benchmarks
# ==================================================

BENCH := $(shell find $(SRC_DIR) -mindepth 1 -maxdepth 1 -type d)
BENCH_EXE := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%.base, $(BENCH))
BENCH_EXE += $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%.smx, $(BENCH))
BENCH_EXE += $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%.smx.nodae, $(BENCH))

.PHONY: clean

bench: $(BENCH_EXE)

clean:
	-rm -rf $(BUILD_DIR)

$(BUILD_DIR)/%: $(BUILD_DIR)/%.o
	mkdir -p $(dir $@)
	$(CROSS_CLANG) $(LDFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.ll
	mkdir -p $(dir $@)
	$(CROSS_LLC) $(LLCFLAGS) $< -o $@

.PRECIOUS: $(BUILD_DIR)/%.smx.ll
$(BUILD_DIR)/%.smx.ll: $(BUILD_DIR)/%.base.ll
	mkdir -p $(dir $@)
	$(CROSS_OPT) $(OPTFLAGS) $< -o $@

.PRECIOUS: $(BUILD_DIR)/%.smx.nodae.ll
$(BUILD_DIR)/%.smx.nodae.ll: $(BUILD_DIR)/%.base.ll
	mkdir -p $(dir $@)
	$(CROSS_OPT) $(OPTFLAGS) $< -o $@ --smx-config-only

.PRECIOUS: $(BUILD_DIR)/%.base.ll
$(BUILD_DIR)/%.base.ll: $(SRC_DIR)/%/main.c
	mkdir -p $(dir $@)
	$(CROSS_CLANG) $(CFLAGS) -S -emit-llvm $< -o $@
