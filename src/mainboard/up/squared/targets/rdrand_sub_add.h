#include "targets.h"

#define CODE_BODY_RDRAND_SUB_ADD \
"rdrand %%ecx;\t\n"

#define PATCH_ADDR 0x7da0
ucode_t ucode_patch[] = {
	/* rcx += rax - rbx (potentially huge jumps) */
	{
		SUB_DSZ64_DRR(TMP0, RAX, RBX),	/* tmp0 = rax - rbx. tmp0 now has per-register flags set */
		ADD_DSZ64_DRR(RCX, TMP0, RCX),
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

		uint32_t operand1 = 0b01, operand2 = 0b01; // Can really be anything, as long as they're equal, I guess?
		uint32_t fault_count = 0;

		// AT&T syntax
		__asm__ __volatile__ (
			"xor %%ecx, %%ecx\t\n"
			REP10(REP100(REP100(CODE_BODY_RDRAND_SUB_ADD))) // 120k iterations
			REP100(REP100(CODE_BODY_RDRAND_SUB_ADD))
			REP100(REP100(CODE_BODY_RDRAND_SUB_ADD))
			: "=c" (fault_count)							// Output operands
			: "a" (operand1),								// Input operands
			  "b" (operand2)
			:												// Clobbered register
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, fault_count);
		// Careful with sending too many bytes in a row or the fifo will fill up
	}
}
