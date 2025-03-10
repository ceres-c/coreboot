/*
 * NOTE: One iteration of this actually takes ~5.55 ms (MILLI!)
 *
 * To be more precise, it's ~5.27 ms when performing an update on top of
 * the same update with a valid RSA signature, and ~5.18 ms when performing
 * an update on top of the same update when the signature check fails.
 *
 * HOWTO: If you want to test loading different microcode updates, you need
 * to change the microcode file embedded in the coreboot image.
 * This can be done through `make menuconfig` and going into
 * `Chipset` -> `Include CPU microcode in CBFS` -> `Include external microcode binary`.
 */

#include <timestamp.h>

#include "targets.h"

#define CODE_BODY_UCODE_UPDATE_DELAY \
"nop;\t\n"

inline static __attribute__((always_inline)) void target_loop(void* uart_base) {
	uint32_t ucode_rev = read_microcode_rev(); // 0x20
	const void *patch = intel_microcode_find();
	const struct microcode *ucode_patch = patch;
	if (!ucode_patch)
		die("microcode: failed because no ucode was found\n");
	if (ucode_rev == ucode_patch->rev)
		die("microcode: failed because already up-to-date\n");
	unsigned long ucode_patch_addr = (unsigned long)ucode_patch + sizeof(struct microcode);

	while (1) {
		uart8250_mem_tx_byte(uart_base, T_CMD_READY);
		uart8250_mem_tx_flush(uart_base);

		/* This code adds a ~30ms wait before the ucode update is actually performed. Do not use this with the glitcher
		 * as it will not handle the 'L' character sent over UART. This is useful ONLY to estimate how much slower the cpu
		 * is when it executes a ucode update straight after power-on.
		 * Spoiler: it is quite a lot slower (10ms vs 6.2ms).
		 */
		// for (int i = 0; i < 0x000FFFFF; i++) {
		// 	__asm__ volatile (
		// 		REP100(CODE_BODY_UCODE_UPDATE_DELAY)
		// 	::
		// 	);
		// }
		// uart8250_mem_tx_byte(uart_base, 'L');

		uint64_t ucode_tsc_start = timestamp_get();
		__asm__ volatile (
			"wrmsr;\t\n"
			REP10(REP100(REP100(CODE_BODY_UCODE_UPDATE_DELAY))) // This is the extra 300us delay, added to allow
			REP10(REP100(REP100(CODE_BODY_UCODE_UPDATE_DELAY))) // the glitcher to restore voltage before UART tx
			REP10(REP100(REP100(CODE_BODY_UCODE_UPDATE_DELAY)))
			REP10(REP100(REP100(CODE_BODY_UCODE_UPDATE_DELAY)))
			REP10(REP100(REP100(CODE_BODY_UCODE_UPDATE_DELAY)))
			: /* No outputs */
			: "c" (IA32_BIOS_UPDT_TRIG), "a" (ucode_patch_addr), "d" (0)
		);
		uint64_t ucode_tsc_end = timestamp_get();
		uint32_t updated_ucode_rev = read_microcode_rev();
		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, updated_ucode_rev);
		if (ucode_tsc_end - ucode_tsc_start > 0xFFFFFFFF)
			/* The board will be reset by the glitcher */
			die("[-] ucode update took %llx cycles > 0xFFFFFFFF\n", ucode_tsc_end - ucode_tsc_start);
		putu32(uart_base, ucode_tsc_end - ucode_tsc_start);
	}
}