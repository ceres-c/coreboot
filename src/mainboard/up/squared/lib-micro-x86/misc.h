#ifndef MISC_H_
#define MISC_H_

#include <stdint.h>

typedef struct {
    uint64_t uop0;
    uint64_t uop1;
    uint64_t uop2;
    uint64_t seqw;
} ucode_t;

#define ARRAY_SZ(arr) \
    ( sizeof(arr) / sizeof(arr[0]) )

#define mfence() asm volatile("mfence\n")
#define lfence() asm volatile("lfence\n")
#define lmfence() asm volatile("lfence\n mfence\n")
#define wbinvd() asm volatile("wbinvd\n")

#endif // MISC_H_
