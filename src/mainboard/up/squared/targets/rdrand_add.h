#include "targets.h"

#define CODE_BODY_RDRAND_ADD \
	"rdrand %%ecx;\t\n"

#define PATCH_ADDR 0x7da0
ucode_t ucode_patch[] = {
	/* rcx += 1 */
	{
		ADD_DSZ64_DRI(RCX, RCX, 1),
		NOP,
		NOP,
		END_SEQWORD
	},
};

inline static __attribute__((always_inline)) void target_loop(void* uart_base) {
	wrmrs_enable_debug();
	do_fix_IN_patch(); /* See 'Backdoor in the Core' talk to see why this is needed */
	apply_patch(RDRAND_XLAT, PATCH_ADDR, ucode_patch, ARRAY_SZ(ucode_patch));

	while (1) {
		uart8250_mem_tx_byte(uart_base, T_CMD_READY);
		uart8250_mem_tx_flush(uart_base);

		uint32_t summation = 0;

		// AT&T syntax
		__asm__ volatile (
			"xor %%ecx, %%ecx\t\n"
			REP10(REP100(REP100(CODE_BODY_RDRAND_ADD))) // 120k iterations
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			: "=c" (summation)								// Output operands
			:												// Input operands
			:												// Clobbered register
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, summation);
		// Careful with sending too many bytes in a row or the fifo will fill up
	}
}
