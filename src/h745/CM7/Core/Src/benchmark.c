#include "benchmark.h"

#include "cmsis_compiler.h"
#include "tim.h"

typedef uint32_t tick;
#define TICK_MAX UINT32_MAX

static tick points[BENCHMARK_NUM_POINTS] = {0};

static inline tick get_current_tick() { return __HAL_TIM_GET_COUNTER(&htim2); }

static inline tick get_ticks_per_sec() { return getTIM2Freq(); }

void benchmark_set_point(enum benchmark_point point) {
  __COMPILER_BARRIER();
  points[point] = get_current_tick();
  __COMPILER_BARRIER();
}

float benchmark_get_result_between_ms(enum benchmark_point st,
                                   enum benchmark_point end) {
  return 1000.0F * (points[end] - points[st]) / get_ticks_per_sec();
}

float benchmark_get_possible_max_ms(void) {
  return 1000.0F * (TICK_MAX) / get_ticks_per_sec();
}