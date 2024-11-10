#ifndef BENCHMARK_H
#define BENCHMARK_H
#ifdef __cplusplus
extern "C" {
#endif

#define USE_BENCHMARK
#ifdef USE_BENCHMARK
#define benchmark_set_point(point) benchmark_set_point_internal(point)
#define benchmark_get_result_between_ms(st, end, min, mean, max) \
  benchmark_get_result_between_ms_internal(st, end, min, mean, max)
#define benchmark_get_possible_max_ms benchmark_get_possible_max_ms_internal
#define benchmark_reset_measurements benchmark_reset_measurements_internal

#else
#define benchmark_set_point(point)
#define benchmark_get_result_between_ms(st, end, min, mean, max)
#define benchmark_get_possible_max_ms
#define benchmark_reset_measurements
#endif

#define BENCHMARK_MEAS_VECTOR_MAX_LEN 16

enum benchmark_point {
  IRQ_BEGIN = 0,
  IRQ_PEND_BOTTOM_HALF,
  IRQ_BOTTOM_HALF_BEGIN,
  IRQ_BEGIN_INVALIDATE,
  IRQ_BEGIN_MEMCPY,
  IRQ_BEGIN_CALLBACK,
  TASK_BEGIN,
  TASK_PREPROC_BEGIN,
  TASK_QUANTIZE_BEGIN,
  TASK_RUN_BEGIN,
  TASK_RUN_BEFORE_INVOKE,
  TASK_RUN_AFTER_INVOKE,
  TASK_RUN_END,
  TASK_POSTPROC_BEGIN,
  TASK_LABLE_PRINT_BEGIN,
  TASK_WAVE_PROC_DONE_BEGIN,
  TASK_END,
  BENCHMARK_NUM_POINTS,
};

void benchmark_set_point_internal(enum benchmark_point point);
// returns the time difference in ms
void benchmark_get_result_between_ms_internal(enum benchmark_point st,
                                               enum benchmark_point end,
                                               float* min, float* mean,
                                               float* max);
void benchmark_reset_measurements_internal(void);
float benchmark_get_possible_max_ms_internal(void);

#ifdef __cplusplus
}
#endif
#endif  // BENCHMARK_H