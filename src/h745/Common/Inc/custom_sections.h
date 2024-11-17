#ifndef CUSTOM_SECTIONS_H
#define CUSTOM_SECTIONS_H

// macro specifying shared variables between the cores, example usage:
//  int SHARED_BSS_VAR(tmp1);
#define SHARED_BSS_VAR(name) name __attribute((section(".shared.bss."#name)))
//  int SHARED_DATA_VAR(tmp2) = <init>;
#define SHARED_DATA_VAR(name) name __attribute((section(".shared.data."#name)))
// int SHARD_BSS_ARR(tmp3, 10);
#define SHARED_BSS_ARR(name, size) name[size] __attribute((section(".shared.bss."#name)))
//  int SHARED_DATA_ARR(tmp4) = <init>;
#define SHARED_DATA_ARR(name, size) name[size] __attribute((section(".shared.data."#name)))

// void print_test_values(void);
void init_custom_sections(void);

#endif // CUSTOM_SECTIONS_H