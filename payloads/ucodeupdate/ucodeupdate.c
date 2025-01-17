/*
 *
 * Copyright (C) 2025 Federico (ceres-c) Cerutti
 *
 */

#include <coreboot_tables.h>
#include <libpayload.h>
#include <arch/rdtsc.h>
#include "06-5c-0a_corrupt_0x1c0.h"

extern uint32_t _06_5c_0a_corrupt_0x1c0_size;
extern const uint8_t _06_5c_0a_corrupt_0x1c0[];

#define IA32_BIOS_UPDT_TRIG		0x79

int main(void)
{
	uint32_t sp; asm volatile ("mov %%esp, %0" : "=r" (sp));

	while (1) {
		uint64_t ucode_tsc_start = rdtsc();
		__asm__ __volatile__ (
			"wrmsr;\t\n"
			: /* No outputs */
			: "c" (IA32_BIOS_UPDT_TRIG), "a" (_06_5c_0a_corrupt_0x1c0), "d" (0)
		);
		uint64_t ucode_tsc_end = rdtsc();
		if (ucode_tsc_end - ucode_tsc_start > 0xFFFFFFFF)
			/* The board will be reset by the glitcher */
			die("[-] ucode update took %llx cycles > 0xFFFFFFFF\n", ucode_tsc_end - ucode_tsc_start);
		printf("Update took %lld cycles\n", ucode_tsc_end - ucode_tsc_start);
	}
}
