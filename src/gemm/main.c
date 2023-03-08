#include <stdio.h>
#include <stdlib.h>

static FILE *f;

static int *access_mat(int *mat, size_t ncol, size_t i, size_t j) {
  return &mat[i * ncol + j];
}

static int *read_mat(size_t nrow, size_t ncol) {
  int *mat = malloc(sizeof(int) * nrow * ncol);
  if (!mat) abort();
  for (size_t i = 0; i < nrow; ++i) {
    for (size_t j = 0; j < ncol; ++j) {
      fscanf(f, "%d", access_mat(mat, ncol, i, j));
    }
  }
  return mat;
}

static int *mat_mul(int *a, int *b, size_t l, size_t m, size_t n) {
  int *c = malloc(sizeof(int) * l * n);
#pragma clang loop smx_streamize(enable)
  for (size_t i = 0; i < l; ++i) {
    for (size_t j = 0; j < n; ++j) {
      int sum = 0;
      for (size_t k = 0; k < m; ++k) {
        sum += *access_mat(a, m, i, k) * *access_mat(b, n, k, j);
      }
      *access_mat(c, n, i, j) = sum;
    }
  }
  return c;
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

  f = fopen(argv[1], "r");
  if (!f) {
    fprintf(stderr, "Failed to open %s!\n", argv[1]);
    exit(1);
  }

  unsigned l, m, n;
  fscanf(f, "%u%u%u", &l, &m, &n);

  int *a = read_mat(l, m);
  int *b = read_mat(m, n);

  int *c = mat_mul(a, b, l, m, n);
  print_mat(c, l, n);

  free(a);
  free(b);
  free(c);
  fclose(f);
  return 0;
}
