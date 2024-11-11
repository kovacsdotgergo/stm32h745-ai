#include "benchmark.h"

#include <assert.h>

#include "cmsis_compiler.h"
#include "macros.h"
#include "tim.h"

typedef uint32_t tick;
#define TICK_MAX UINT32_MAX
typedef uint64_t tick_sum;
#define TICK_SUM_MAX UINT64_MAX

static_assert(BENCHMARK_MEAS_VECTOR_MAX_LEN <= TICK_SUM_MAX / TICK_MAX);
struct meas_vector {
  tick arr[BENCHMARK_MEAS_VECTOR_MAX_LEN];
  size_t len;
};

static struct meas_vector points[BENCHMARK_NUM_POINTS] = {0};

static inline tick get_current_tick() { return __HAL_TIM_GET_COUNTER(&htim2); }

static inline tick get_ticks_per_sec() { return getTIM2Freq(); }

void benchmark_set_point_internal(enum benchmark_point point) {
  __COMPILER_BARRIER();
  struct meas_vector* cur_point = &points[point];
  if (cur_point->len < BENCHMARK_MEAS_VECTOR_MAX_LEN) {
    cur_point->arr[cur_point->len++] = get_current_tick();
  }
  __COMPILER_BARRIER();
}

void benchmark_get_result_between_ms_internal(enum benchmark_point st,
                                     enum benchmark_point end, float* min,
                                     float* mean, float* max) {
  assert(points[end].len == points[st].len);

  size_t len = points[end].len;
  tick_sum sum = 0;
  tick_sum lmin = TICK_SUM_MAX;
  tick_sum lmax = 0;
  for (size_t i = 0; i < len; ++i) {
    tick_sum diff = points[end].arr[i] - points[st].arr[i];
    sum += diff;
    lmin = diff < lmin ? diff : lmin;
    lmax = lmax < diff ? diff : lmax;
  }

  *mean = 1000.0F * (sum / len) / get_ticks_per_sec();
  *min = 1000.0F * lmin / get_ticks_per_sec();
  *max = 1000.0F * lmax / get_ticks_per_sec();
}

void benchmark_reset_measurements_internal(void) {
  for (size_t i = 0; i < ARRAY_LEN(points); ++i) {
    points[i].len = 0;
  }
}

float benchmark_get_possible_max_ms_internal(void) {
  return 1000.0F * (TICK_MAX) / get_ticks_per_sec();
}
