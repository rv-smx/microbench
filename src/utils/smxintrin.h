#ifndef SMX_SMXINTRIN_H_
#define SMX_SMXINTRIN_H_

/// @brief Stop conditions of induction variable stream.
enum {
  SMX_STOP_COND_GT = 0,
  SMX_STOP_COND_GE = 1,
  SMX_STOP_COND_LT = 2,
  SMX_STOP_COND_LE = 3,
  SMX_STOP_COND_GTU = 4,
  SMX_STOP_COND_GEU = 5,
  SMX_STOP_COND_LTU = 6,
  SMX_STOP_COND_LEU = 7,
  SMX_STOP_COND_EQ = 8,
  SMX_STOP_COND_NE = 9,
};

/// @brief Dependent stream kind in address factor.
enum {
  SMX_STREAM_KIND_IV = 0,
  SMX_STREAM_KIND_MS = 1,
};

#endif  // SMX_SMXINTRIN_H_
