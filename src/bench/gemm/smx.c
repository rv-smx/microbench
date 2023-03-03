#include <stdio.h>
#include <stdlib.h>

#include "smxintrin.h"

static FILE *f;

static int *access_mat(int *mat, unsigned ncol, unsigned i, unsigned j) {
  return &mat[i * ncol + j];
}

static int *read_mat(unsigned nrow, unsigned ncol) {
  int *mat = malloc(sizeof(int) * nrow * ncol);
  if (!mat) abort();
  for (unsigned i = 0; i < nrow; ++i) {
    for (unsigned j = 0; j < ncol; ++j) {
      fscanf(f, "%d", access_mat(mat, ncol, i, j));
    }
  }
  return mat;
}

static int *mat_mul(int *a, int *b, unsigned l, unsigned m, unsigned n) {
  int *c = malloc(sizeof(int) * l * n);
  __builtin_riscv_smx_cfg_ms(c, 4 * n, 0, SMX_STREAM_KIND_IV, 1, 4);
  __builtin_riscv_smx_cfg_addr(4, 1, SMX_STREAM_KIND_IV, 0, 0, 0);
  __builtin_riscv_smx_ready();
  do {
    do {
      int sum = 0;
      do {
        sum += __builtin_riscv_smx_load_i32(0, 0) *
               __builtin_riscv_smx_load_i32(1, 0);
      } while (__builtin_riscv_smx_step(2) != __builtin_riscv_smx_stop_val(2));
      __builtin_riscv_smx_store_i32(2, 0, sum);
    } while (__builtin_riscv_smx_step(1) != __builtin_riscv_smx_stop_val(1));
  } while (__builtin_riscv_smx_step(0) != __builtin_riscv_smx_stop_val(0));
  __builtin_riscv_smx_end();
  return c;
}

static void print_mat(int *mat, unsigned nrow, unsigned ncol) {
  for (unsigned i = 0; i < nrow; ++i) {
    for (unsigned j = 0; j < ncol; ++j) {
      printf("%d ", *access_mat(mat, ncol, i, j));
    }
    printf("\n");
  }
}

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s INPUT\n", argv[0]);
    exit(1);
  }

  f = fopen(argv[1], "r");
  if (!f) {
    fprintf(stderr, "Failed to open %s!\n", argv[1]);
    exit(1);
  }

  unsigned l, m, n;
  fscanf(f, "%u%u%u", &l, &m, &n);
  if (!l || !m || !n) exit(0);

  __builtin_riscv_smx_cfg_iv(0, 1, l, SMX_STOP_COND_GEU);
  __builtin_riscv_smx_cfg_iv(0, 1, n, SMX_STOP_COND_GEU);
  __builtin_riscv_smx_cfg_iv(0, 1, m, SMX_STOP_COND_GEU);

  int *a = read_mat(l, m);
  __builtin_riscv_smx_cfg_ms(a, 4 * m, 0, SMX_STREAM_KIND_IV, 1, 4);
  __builtin_riscv_smx_cfg_addr(4, 2, SMX_STREAM_KIND_IV, 0, 0, 0);
  int *b = read_mat(m, n);
  __builtin_riscv_smx_cfg_ms(b, 4 * n, 2, SMX_STREAM_KIND_IV, 1, 4);
  __builtin_riscv_smx_cfg_addr(4, 1, SMX_STREAM_KIND_IV, 0, 0, 0);

  int *c = mat_mul(a, b, l, m, n);
  print_mat(c, l, n);

  free(a);
  free(b);
  free(c);
  fclose(f);
  return 0;
}
