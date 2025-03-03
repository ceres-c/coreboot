#include "targets.h"

#define CODE_BODY_RDRAND_CMP_NE \
"rdrand %%ecx;\t\n"

#define PATCH_ADDR 0x7da0
ucode_t ucode_patch[] = {
	{ /* R64SRC := RAX == RBX ? 2 : 3 */
		/* Note that without SYNCs, the result of upcoming architectural operations can be overwritten in weird ways */
		SUB_DSZ64_DRR(TMP0, RAX, RBX),	/* tmp0 = rax - rbx. tmp0 now has per-register flags set */
		UJMPCC_DIRECT_NOTTAKEN_CONDNZ_RI(TMP0, (PATCH_ADDR + 0x04)),
		ADD_DSZ64_DRI(R64SRC, R64SRC, 0),
		( SEQ_UEND0(2) | SEQ_NEXT | SEQ_SYNCFULL(1) )	// 1. End on uop 2 (if executed)
														// 2. Otherwise continue to the next triad
														// 3. Force OOOE to wait for the UJUMP to be evaluated,
														// then pick the right branch (both are speculatively
														// executed, but only one is retired, aka committed)
		// SYNCMARK and SYNCWAIT are also an option, but they require one extra ucode triad:
		//	- one to write 0x01 to R64SRC
		//	- one to write 0x02 to R64SRC
		// They both use a seqword like ( SEQ_UEND0(0) | SEQ_NEXT | SEQ_SYNCWAIT(0) )
	}, {
		ADD_DSZ64_DRI(R64SRC, R64SRC, 1),
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
		__asm__ __volatile__ (
			"xor %%ecx, %%ecx\t\n"
			REP100(CODE_BODY_RDRAND_CMP_NE) // 400 iterations
			REP100(CODE_BODY_RDRAND_CMP_NE)
			REP100(CODE_BODY_RDRAND_CMP_NE)
			REP100(CODE_BODY_RDRAND_CMP_NE)
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
