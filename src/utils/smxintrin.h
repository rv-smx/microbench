#ifndef SMX_SMXINTRIN_H_
#define SMX_SMXINTRIN_H_

#include <stddef.h>

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

/// Some magics.
#define __SMX_HELPER_CONCAT0(x, y) x##y
#define __SMX_HELPER_CONCAT(x, y) __SMX_HELPER_CONCAT0(x, y)
#define __SMX_HELPER_LOOP_LABEL __SMX_HELPER_CONCAT(__smx_loop_, __LINE__)
#define __SMX_HELPER_IV_VAR_NAME __SMX_HELPER_CONCAT(__smx_iv_var_, __LINE__)
#define __SMX_HELPER_DO_WHILE_VAR(iv, var, init, body, label, iv_var) \
  do {                                                                \
  label:                                                              \
    (void)0;                                                          \
    size_t var = (init);                                              \
    body size_t iv_var = __builtin_riscv_smx_step_bl(iv);             \
    var = iv_var;                                                     \
    if (iv_var == __builtin_riscv_smx_b_cond()) goto label;           \
  } while (0)

/// Creates a SMX-style do-while loop.
#define SMX_DO_WHILE(iv, body)                                           \
  do {                                                                   \
  __SMX_HELPER_LOOP_LABEL:                                               \
    (void)0;                                                             \
    body if (__builtin_riscv_smx_step_bl(iv) ==                          \
             __builtin_riscv_smx_b_cond()) goto __SMX_HELPER_LOOP_LABEL; \
  } while (0)

/// Creates a SMX-style do-while loop with induction variable.
#define SMX_DO_WHILE_VAR(iv, var, init, body)                               \
  do {                                                                      \
  __SMX_HELPER_LOOP_LABEL:                                                  \
    (void)0;                                                                \
    size_t var = (init);                                                    \
    body size_t __SMX_HELPER_IV_VAR_NAME = __builtin_riscv_smx_step_bl(iv); \
    var = __SMX_HELPER_IV_VAR_NAME;                                         \
    if (__SMX_HELPER_IV_VAR_NAME == __builtin_riscv_smx_b_cond())           \
      goto __SMX_HELPER_LOOP_LABEL;                                         \
  } while (0)

#endif  // SMX_SMXINTRIN_H_
