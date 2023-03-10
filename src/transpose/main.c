#include <stdio.h>
#include <stdlib.h>

static int *access_mat(int *mat, size_t ncol, size_t i, size_t j) {
  return &mat[i * ncol + j];
}

static int *read_mat(FILE *f, size_t nrow, size_t ncol) {
  int *mat = malloc(sizeof(int) * nrow * ncol);
  if (!mat) abort();
  for (size_t i = 0; i < nrow; ++i) {
    for (size_t j = 0; j < ncol; ++j) {
      fscanf(f, "%d", access_mat(mat, ncol, i, j));
    }
  }
  return mat;
}

static void print_mat(int *mat, size_t nrow, size_t ncol) {
  for (size_t i = 0; i < nrow; ++i) {
    for (size_t j = 0; j < ncol; ++j) {
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

  FILE *input = fopen(argv[1], "r");
  if (!input) {
    fprintf(stderr, "Failed to open %s!\n", argv[1]);
    exit(1);
  }

  unsigned m, n;
  fscanf(input, "%u%u", &m, &n);

  int *mat = read_mat(input, m, n);

  int *result = malloc(sizeof(int) * n * m);
#pragma clang loop smx_streamize(enable)
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      *access_mat(result, m, i, j) = *access_mat(mat, n, j, i);
    }
  }

  print_mat(result, m, n);

  free(mat);
  free(result);
  fclose(input);
  return 0;
}
