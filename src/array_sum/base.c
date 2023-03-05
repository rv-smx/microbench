#include <stdio.h>
#include <stdlib.h>

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

  int sum = 0;
  for (size_t i = 0; i < len; ++i) {
    sum += arr[i];
  }

  printf("%d\n", sum);
  fclose(input);
  return 0;
}
