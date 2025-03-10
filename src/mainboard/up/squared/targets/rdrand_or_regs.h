#include "targets.h"

#define CODE_BODY_RDRAND_OR_REGS \
	"rdrand %%ecx;\t\n"

#define PATCH_ADDR 0x7da0
ucode_t ucode_patch[] = {
	/* rcx = rcx */
	/* Move around the current value of rcx without explicitly changing it, then store back to rcx. Test issues with register file */
	{
		OR_DSZ32_DRI(TMP0, RCX, 0),
		OR_DSZ32_DRI(TMP1, TMP0, 0),
		OR_DSZ32_DRI(TMP2, TMP1, 0),
		NOP_SEQWORD
	},
	{
		OR_DSZ32_DRI(TMP3, TMP2, 0),
		OR_DSZ32_DRI(TMP4, TMP3, 0),
		OR_DSZ32_DRI(TMP5, TMP4, 0),
		NOP_SEQWORD
	},
	{
		OR_DSZ32_DRI(TMP6, TMP5, 0),
		OR_DSZ32_DRI(TMP7, TMP6, 0),
		OR_DSZ32_DRI(TMP8, TMP7, 0),
		NOP_SEQWORD
	},
	{
		OR_DSZ32_DRI(TMP9, TMP8, 0),
		OR_DSZ32_DRI(TMP10, TMP9, 0),
		OR_DSZ32_DRI(RCX, TMP10, 0),
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

		uint32_t input = 0xFFFFFFFF, output = 0;

		// AT&T syntax
		__asm__ volatile (
			REP100(REP100(CODE_BODY_RDRAND_OR_REGS))		// 90k iterations
			REP100(REP100(CODE_BODY_RDRAND_OR_REGS))
			REP100(REP100(CODE_BODY_RDRAND_OR_REGS))
			REP100(REP100(CODE_BODY_RDRAND_OR_REGS))
			REP100(REP100(CODE_BODY_RDRAND_OR_REGS))
			REP100(REP100(CODE_BODY_RDRAND_OR_REGS))
			REP100(REP100(CODE_BODY_RDRAND_OR_REGS))
			REP100(REP100(CODE_BODY_RDRAND_OR_REGS))
			REP100(REP100(CODE_BODY_RDRAND_OR_REGS))
			: "=c" (output)									// Output operands
			: "c" (input)									// Input operands
			:												// Clobbered register
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, output);
		// Careful with sending too many bytes in a row or the fifo will fill up
	}
}
