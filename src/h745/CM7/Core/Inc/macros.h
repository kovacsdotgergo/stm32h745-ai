#ifndef MACROS_H
#define MACROS_H

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#define BUFFER_SIZE_4_BYTE_CEIL(size) (((size) + 3) / 4 * 4)

#endif // MACROS_H