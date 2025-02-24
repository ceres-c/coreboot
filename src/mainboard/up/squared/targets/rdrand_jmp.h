#include "targets.h"

#define PATCH_ADDR 0x7da0
ucode_t ucode_patch[] = {
	/* rcx := rax == rbx ? 1 : 2 */
	{
		SUB_DSZ64_DRR(TMP0, RAX, RBX),
		UJMPCC_DIRECT_NOTTAKEN_CONDNZ_RI(TMP0, (PATCH_ADDR + 0x04)),
		ZEROEXT_DSZ64_DI(RCX, 1),
		(SEQ_UEND0(2) | SEQ_NEXT | SEQ_SYNCFULL(1) )
	}, {
		ZEROEXT_DSZ64_DI(RCX, 2),
		NOP,
		NOP,
		END_SEQWORD
	}
};

inline static __attribute__((always_inline)) void target_loop(void* uart_base) {
	wrmrs_enable_debug();
	do_fix_IN_patch(); /* See 'Backdoor in the Core' talk to see why this is needed */
	apply_patch(RDRAND_XLAT, PATCH_ADDR, ucode_patch, ARRAY_SZ(ucode_patch));

	uint32_t count = 0;
	while (1) {
		uart8250_mem_tx_byte(uart_base, T_CMD_READY);
		uart8250_mem_tx_flush(uart_base);

		uint32_t operand1 = count % 2, operand2 = 1, output = 0;
		__asm__ __volatile__ (
			"xor %%ecx, %%ecx;\t\n"
			"rdrand %%ecx;\t\n"
			: "=c" (output)
			: "a" (operand1),
			  "b" (operand2)
			:
		);
		count++;

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, output);
		// Careful with sending too many bytes in a row or the fifo will fill up
	}
}
