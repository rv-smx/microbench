#include <stdio.h>
#include <stdlib.h>

#include "smxintrin.h"

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s INPUT\n", argv[0]);
    exit(1);
  }

  FILE *input = fopen(argv[1], "r");
  if (!input) {
    fprintf(stderr, "Failed to open %s!\n", argv[1]);
    exit(1);
  }

  unsigned len;
  fscanf(input, "%u", &len);
  int *arr = malloc(sizeof(int) * len);
  for (size_t i = 0; i < len; ++i) {
    fscanf(input, "%d", &arr[i]);
  }

  __builtin_riscv_smx_cfg_iv(0, 1, len, SMX_STOP_COND_GEU);
  __builtin_riscv_smx_cfg_ms(arr, 4, 0, SMX_STREAM_KIND_IV, 1, 4);
  __builtin_riscv_smx_ready();

  int sum = 0;
  if (len) {
    SMX_DO_WHILE(0, { sum += __builtin_riscv_smx_load_i32(0, 0); });
  }

  __builtin_riscv_smx_end();

  printf("%d\n", sum);
  fclose(input);
  return 0;
}
