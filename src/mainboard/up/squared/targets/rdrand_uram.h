#include "targets.h"

#define PATCH_ADDR 0x7da0
ucode_t ucode_patch[] = {
	/* Read/write URAM test */
	{
		ZEROEXT_DSZ64_DI(TMP0, 0x0007),
		CONCAT_DSZ16_DRI(TMP0, TMP0, 0xFFFF),	// TMP0 := 0x0007FFFF
		ZEROEXT_DSZ64_DI(TMP1, 0x01),
		#error "Must first zero out TMP3"
		NOP_SEQWORD,
	}, {
		WRITEURAM_RI(TMP1, 0x48),				// Pray nobody uses this address
		READURAM_DI(TMP2, 0x48),
		ADD_DSZ64_DRR(TMP3, TMP3, TMP2),
		NOP_SEQWORD,
	}, {
		SUB_DSZ64_DIR(TMP0, 1, TMP0),			// TMP0 := TMP0 - 1
		UJMPCC_DIRECT_NOTTAKEN_CONDNZ_RI(TMP0, (PATCH_ADDR + 0x04)),
		MOVE_DSZ32_DR(R64SRC, TMP3),
		( SEQ_NOP| SEQ_NEXT | SEQ_SYNCFULL(1) )
	}
};

inline static __attribute__((always_inline)) void target_loop(void* uart_base) {
	wrmrs_enable_debug();
	do_fix_IN_patch(); /* See 'Backdoor in the Core' talk to see why this is needed */
	apply_patch(RDRAND_XLAT, PATCH_ADDR, ucode_patch, ARRAY_SZ(ucode_patch));

	while (1) {
		uart8250_mem_tx_byte(uart_base, T_CMD_READY);
		uart8250_mem_tx_flush(uart_base);

		uint32_t output = 0;
			__asm__ volatile (
				"xor %%ecx, %%ecx;\t\n"
				"rdrand %%ecx;\t\n"
				: "=c" (output)
				:
				:
			);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, output); // 0x0007FFFF
		// Careful with sending too many bytes in a row or the fifo will fill up
	}
}
