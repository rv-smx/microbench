#include <stdio.h>
#include <stdlib.h>

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
  unsigned i = 0;
#pragma nounroll
  do {
    unsigned j = 0;
#pragma nounroll
    do {
      int sum = 0;
      unsigned k = 0;
#pragma nounroll
      do {
        sum += *access_mat(a, m, i, k) * *access_mat(b, n, k, j);
        ++k;
      } while (k < m);
      *access_mat(c, n, i, j) = sum;
      ++j;
    } while (j < n);
    ++i;
  } while (i < l);
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
