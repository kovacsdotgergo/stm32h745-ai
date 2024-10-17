#ifndef BENCHMARK_H
#define BENCHMARK_H
#ifdef __cplusplus
extern "C" {
#endif

enum benchmark_point {
  BEGIN_PREPOC = 0,
  PREPROC_F32,
  PREPROC_Q31,
  PREPROC_Q15,
  BEGIN_QUANTIZE,
  QUNATIZE,
  QUANTIZE_NAIVE,
  BEGIN_RUN,
  INSIDE_LOAD_MODEL,
  INSIDE_SETUP,
  INSIDE_BEFORE_INVOKE,
  INSIDE_AFTER_INVOKE,
  END_RUN,
  BENCHMARK_NUM_POINTS,
};

void benchmark_set_point(enum benchmark_point point);
// returns the time difference in ms
float benchmark_get_result_between_ms(enum benchmark_point st,
                                      enum benchmark_point end);
float benchmark_get_possible_max_ms(void);

#ifdef __cplusplus
}
#endif
#endif  // BENCHMARK_H