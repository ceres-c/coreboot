#include "targets.h"

#define CODE_BODY_RDRAND_MOVE_REGS \
	"rdrand %%ecx;\t\n"

#define PATCH_ADDR 0x7da0
ucode_t ucode_patch[] = {
	/* rcx = rcx */
	/* Move around the current value of rcx without explicitly changing it, then store back to rcx. Test issues with register file */
	{
		ZEROEXT_DSZ32_DR(TMP0, RCX),
		ZEROEXT_DSZ32_DR(TMP1, TMP0),
		ZEROEXT_DSZ32_DR(TMP2, TMP1),
		NOP_SEQWORD
	},
	{
		ZEROEXT_DSZ32_DR(TMP3, TMP2),
		ZEROEXT_DSZ32_DR(TMP4, TMP3),
		ZEROEXT_DSZ32_DR(TMP5, TMP4),
		NOP_SEQWORD
	},
	{
		ZEROEXT_DSZ32_DR(TMP6, TMP5),
		ZEROEXT_DSZ32_DR(TMP7, TMP6),
		ZEROEXT_DSZ32_DR(TMP8, TMP7),
		NOP_SEQWORD
	},
	{
		ZEROEXT_DSZ32_DR(TMP9, TMP8),
		ZEROEXT_DSZ32_DR(TMP10, TMP9),
		ZEROEXT_DSZ32_DR(RCX, TMP10),
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
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))		// 90k iterations
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			: "=c" (output)									// Output operands
			: "c" (input)									// Input operands
			:												// Clobbered register
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, output);
		// Careful with sending too many bytes in a row or the fifo will fill up
	}
}
