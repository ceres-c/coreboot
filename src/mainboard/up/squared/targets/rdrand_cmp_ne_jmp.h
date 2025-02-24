#include "targets.h"

#define PATCH_ADDR 0x7da0
ucode_t ucode_patch[] = {
	#error RDRAND_CMP with jumps does not work, currently. Jumps are broken for unknown reasons in coreboot.
	/* rcx += rax != rbx (with jumps) */
	{
		SUB_DSZ32_DRR(TMP0, RAX, RBX),	/* tmp0 = rax - rbx. tmp0 now has per-register flags set */
		UJMPCC_DIRECT_NOTTAKEN_CONDNZ_RI(TMP0, (PATCH_ADDR + 0x04)), // NOTE: This is the jump that breaks everything. Works in linux
		NOP,
		END_SEQWORD
	},
	{
		ADD_DSZ32_DRI(RCX, RCX, 1),
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


	}
}
