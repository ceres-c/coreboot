/*
 * Measure and print ucode update time.
 *
 * NOTE: Loggin must be set to BIOS_INFO to see anything
 */

#include <timestamp.h>

#include "targets.h"

inline static __attribute__((always_inline)) void target_loop(void* uart_base) {
	/* No need for spinlocks here, I assume. We are still running a single core */
	uint32_t ucode_FIT_rev = read_microcode_rev(); // 0x20
	printk(BIOS_INFO, "Microcode FIT revision: 0x%x\n", ucode_FIT_rev);

	const void *cbfs_ucode = intel_microcode_find();
	const struct microcode *cbfs_ucode_patch = (struct microcode*)cbfs_ucode;
	if (!cbfs_ucode_patch)
		die("microcode: failed because no ucode was found\n");
	if (ucode_FIT_rev == cbfs_ucode_patch->rev)
		die("microcode: Update skipped, already up-to-date\n");
	unsigned long ucode_print_patch_addr = (unsigned long)cbfs_ucode_patch + sizeof(struct microcode);
	uint64_t ucode_print_tsc = timestamp_get();
	__asm__ volatile (
		"wrmsr"
		: /* No outputs */
		: "c" (IA32_BIOS_UPDT_TRIG), "a" (ucode_print_patch_addr), "d" (0)
	);
	uint64_t ucode_print_tsc2 = timestamp_get();
	ucode_FIT_rev = read_microcode_rev();
	printk(BIOS_INFO, "Microcode CBFS revision: 0x%x\n", ucode_FIT_rev);
	printk(BIOS_INFO, "Microcode update took %lld cycles\n", ucode_print_tsc2 - ucode_print_tsc);
}
