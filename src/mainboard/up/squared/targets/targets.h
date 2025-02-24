#ifndef TARGETS_H
#define TARGETS_H

#include <stdint.h>
#include <console/uart8250mem.h>
#include <commonlib/loglevel.h>

#include "../red_unlock.h"
#include "../lib-micro-x86/lib-micro-minimal.h"

#define MAGIC_UNLOCK 0x200 // See "Undocumented x86 Instructions..." by Ermolov et al.

#define REP10(BODY) BODY BODY BODY BODY BODY BODY BODY BODY BODY BODY
#define REP100(BODY) REP10(REP10(BODY))

inline static __attribute__((always_inline)) void target_loop(void* uart_base);
inline static __attribute__((always_inline)) void wrmrs_enable_debug(void) {
	/* Enable ucode debug */
	unsigned int low = 0, high = 0;
	__asm__ volatile ("wrmsr" : : "a" (MAGIC_UNLOCK), "d" (0), "c" (APL_UCODE_CRBUS_UNLOCK));
	__asm__ volatile ("rdmsr" : "=a" (low), "=d" (high) : "c" (APL_UCODE_CRBUS_UNLOCK));
	if (high != 0 || low != MAGIC_UNLOCK) {
		die("\tFailed to write APL_UCODE_CRBUS_UNLOCK MSR\n");
	}
}
inline static __attribute__((always_inline)) void apply_patch(uint32_t ucode_msrom_addr, uint32_t ucode_msram_addr, ucode_t *ucode_patch, int triad_count) {
	/* Install the patch.
	 *
	 * Args:
	 *  - ucode_msrom_addr: The source address (in ucode ROM) for match&patch
	 *    registers (see XLAT macros to patch full instructions)
	 *  - ucode_msram_addr: Where to put the patch (in ucode RAM)
	 *  - ucode_patch: The patch to apply
	 */
	patch_ucode(ucode_msram_addr, ucode_patch, triad_count);
	hook_match_and_patch(0, ucode_msrom_addr, ucode_msram_addr);
	printk(BIOS_INFO, "RDRAND patched\n");
}

#endif
