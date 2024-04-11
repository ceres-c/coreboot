#ifndef UDBG_H_
#define UDBG_H_

#include <stdint.h>
#include "misc.h"

typedef struct {
	uint64_t value;
	uint32_t status;
} u_result_t;

__attribute__((always_inline))
inline static u_result_t udbgrd(uint32_t type, uint32_t addr) {
	uint32_t res_low, res_high;
	lmfence();
	asm volatile(
		".byte 0x0F, 0x0E\n\t"
		: "=d" (res_low)
		, "=b" (res_high)
		: "a" (addr)
		, "c" (type)
	);
	lmfence();
	u_result_t res;
	res.value = ((uint64_t)res_high << 32) | res_low;
	res.status = res_low;
	return res;
}

__attribute__((always_inline))
inline static u_result_t udbgwr(uint32_t type, uint32_t addr, uint64_t value) {
	uint32_t value_low = (uint32_t)(value & 0xFFFFFFFF);
	uint32_t value_high = (uint32_t)(value >> 32);
	uint32_t res_low, res_high;
	lmfence();
	asm volatile(
		".byte 0x0F, 0x0F\n\t"
		: "=d" (res_low)
		, "=b" (res_high)
		: "a" (addr)
		, "c" (type)
		, "d" (value_low)
		, "b" (value_high)
	);
	lmfence();
	u_result_t res;
	res.value = ((uint64_t)res_high << 32) | res_low;
	res.status = res_low;
	return res;
}

__attribute__((always_inline))
inline static uint32_t ucode_invoke_2(uint32_t addr, uint32_t arg1, uint32_t arg2) {
	uint32_t eax = addr, ecx = 0xD8;
	lmfence();
	asm volatile(
		".byte 0x0F, 0x0F\n\t"
		: "+a" (eax)
		, "+c" (ecx)
		, "+rdi" (arg1)
		, "+rsi" (arg2)
		:
		: "ebx", "edx" //, "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
					   // It shouldn't (TM) matter if these are clobbered, as we're in x86 anyway
	);
	lmfence();
	return eax;
}

#define SIMPLERD(name, type) \
__attribute__((always_inline)) \
inline static uint64_t name(uint32_t addr) { \
    return (uint64_t)udbgrd(type, addr).value; \
}

SIMPLERD(crbus_read, 0x00)
#undef SIMPLERD

#define SIMPLEWR(name, type)     \
__attribute__((always_inline)) \
inline static void name(uint32_t addr, uint64_t value) { \
	udbgwr(type, addr, value); \
}

SIMPLEWR(crbus_write, 0x00)
#undef SIMPLEWR

#endif // UDBG_H_
