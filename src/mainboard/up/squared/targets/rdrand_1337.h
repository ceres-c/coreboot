#include "targets.h"
#define CODE_BODY_RDRAND_1337 \
"rdrand %%ecx;\t\n"

#define PATCH_ADDR 0x7da0
ucode_t ucode_patch[] = {
	{
		ZEROEXT_DSZ32_DI(RCX, 0x1337), /* Write zero extended */
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

		uint32_t result = 0;

		// AT&T syntax
		__asm__ volatile (
		"xor %%ecx, %%ecx\t\n"
		REP10(REP100(REP100(CODE_BODY_RDRAND_1337))) // 120k iterations
		REP100(REP100(CODE_BODY_RDRAND_1337))
		REP100(REP100(CODE_BODY_RDRAND_1337))
		: "=c" (result)									// Output operands
		:												// Input operands
		:												// Clobbered register
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, result);
		// Careful with sending too many bytes in a row or the fifo will fill up
	}
}
