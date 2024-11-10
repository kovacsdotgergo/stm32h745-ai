#include "custom_sections.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// shared section
extern char _load_start_shared_data, _start_shared_data, _end_shared_data;
extern char _start_shared_bss, _end_shared_bss;
// non cacheable section
extern char _load_start_non_cacheable_data, _start_non_cacheable_data,
    _end_non_cacheable_data;
extern char _start_non_cacheable_bss, _end_non_cacheable_bss;
// speedy mem
extern char _load_start_speedy_mem_data, _start_speedy_mem_data,
    _end_speedy_mem_data;
extern char _start_speedy_mem_bss, _end_speedy_mem_bss;


static void zero_init(char* from, char* to) { memset(from, 0x00, to - from); }

static void copy_init(char* from, char* to, const char* load_from) {
  memcpy(from, load_from, to - from);
}

// data
uint32_t tst0 __attribute__((section(".shared.data")))  = 0xffffffff;
uint32_t tst1 __attribute__((section(".shared.data")));
uint32_t tst2 __attribute__((section(".non_cacheable.data"))) = 0x77777777;
uint32_t tst3 __attribute__((section(".non_cacheable.data"))) = 0;
uint32_t tst6 __attribute__((section(".speedy_mem.data"))) = 0xaaaaaaaa;
uint32_t tst7 __attribute__((section(".speedy_mem.data"))) = 0;
// bss
uint32_t tst4 __attribute__((section(".non_cacheable.bss"))) ;
uint32_t tst5 __attribute__((section(".shared.bss")));
uint32_t tst8 __attribute__((section(".speedy_mem.bss")));

void print_test_values(void) {
  printf("%lx, %lx, %lx, %lx, %lx, %lx, (junk)%lx (junk)%lx, (junk)%lx\r\n",
         tst0, tst1, tst2, tst3, tst6, tst7, tst4, tst5, tst8);
}

void init_custom_sections(void) {
  zero_init(&_start_non_cacheable_bss, &_end_non_cacheable_bss);
  copy_init(&_start_non_cacheable_data, &_end_non_cacheable_data,
            &_load_start_non_cacheable_data);
  zero_init(&_start_shared_bss, &_end_shared_bss);
  copy_init(&_start_shared_data, &_end_shared_data, &_load_start_shared_data);
  zero_init(&_start_speedy_mem_bss, &_end_speedy_mem_bss);
  copy_init(&_start_speedy_mem_data, &_end_speedy_mem_data,
            &_load_start_speedy_mem_data);
}
