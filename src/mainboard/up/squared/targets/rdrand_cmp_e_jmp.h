#include "targets.h"

#define CODE_BODY_RDRAND_CMP_E \
"rdrand %%ecx;\t\n"

#define PATCH_ADDR 0x7da0
ucode_t ucode_patch[] = {
	{ /* R64SRC := RAX == RBX ? 0 : 1 */
		/* Note that without SYNCs, the result of upcoming architectural operations can be overwritten in weird ways */
		// See rdrand_cmp_ne_jmp.h for explanation
		SUB_DSZ64_DRR(TMP0, RAX, RBX),	/* tmp0 = rax - rbx. tmp0 now has per-register flags set */
		UJMPCC_DIRECT_NOTTAKEN_CONDZ_RI(TMP0, (PATCH_ADDR + 0x04)),
		ADD_DSZ64_DRI(R64SRC, R64SRC, 1),
		( SEQ_UEND0(2) | SEQ_NEXT | SEQ_SYNCFULL(1) )
	}, {
		ADD_DSZ64_DRI(R64SRC, R64SRC, 0),
		NOP,
		NOP,
		( SEQ_UEND0(0) | SEQ_NEXT | SEQ_NOSYNC )
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
		uint32_t result = 0;

		// AT&T syntax
		__asm__ volatile (
			"xor %%ecx, %%ecx\t\n" // Before this command, ecx = 0x00000446
			REP100(REP100(CODE_BODY_RDRAND_CMP_E)) // 40000 iterations
			REP100(REP100(CODE_BODY_RDRAND_CMP_E))
			REP100(REP100(CODE_BODY_RDRAND_CMP_E))
			REP100(REP100(CODE_BODY_RDRAND_CMP_E))
			: "=c" (result)
			: "a" (operand1),
			  "b" (operand2)
			:
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, result);
		// Careful with sending too many bytes in a row or the fifo will fill up
	}
}
