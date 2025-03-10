#include "targets.h"

#define CODE_BODY_RDRAND_CMP_NE \
"rdrand %%ecx;\t\n"

#define PATCH_ADDR 0x7da0
ucode_t ucode_patch[] = {
	/* rcx += rax != rbx (with conditional set) */
	{
		SUB_DSZ32_DRR(TMP0, RAX, RBX),	/* tmp0 = rax - rbx. tmp0 now has per-register flags set */
		SETCC_CONDNZ_DR(TMP1, TMP0),
		ADD_DSZ32_DRR(RCX, TMP1, RCX),	/* NOTE: Both ADD and SUB 64 bit opcodes work just as well */
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

		uint32_t operand1 = 0xAAAAAAAA, operand2 = 0xAAAAAAAA; // Can really be anything, as long as they're equal, I guess?
		uint32_t diff_count = 0;

		// AT&T syntax
		__asm__ volatile (
			"xor %%ecx, %%ecx\t\n"
			REP10(REP100(REP100(CODE_BODY_RDRAND_CMP_NE))) // 120k iterations
			REP100(REP100(CODE_BODY_RDRAND_CMP_NE))
			REP100(REP100(CODE_BODY_RDRAND_CMP_NE))
			: "=c" (diff_count)								// Output operands
			: "a" (operand1),								// Input operands
			  "b" (operand2)
			: 												// Clobbered register
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, diff_count);
		// Careful with sending too many bytes in a row or the fifo will fill up
	}
}
