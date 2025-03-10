#include "targets.h"

#define PATCH_ADDR 0x7da0

inline static __attribute__((always_inline)) void target_loop(void* uart_base) {
	ucode_t ucode_patch[] = {
		/* Loopy mcloopface */
		{
			// MOVEFROMCREG_DSZ64_DI(R12, 0x38c),	// Pause frontend (either this or the NOP below)
			ZEROEXT_DSZ64_DI(TMP0, 0x000D),
			CONCAT_DSZ16_DRI(TMP0, TMP0, 0xFFFF),	// TMP0 := 0x000DFFFF
			NOP,
			NOP_SEQWORD,
		}, {
			SUB_DSZ64_DIR(TMP0, 1, TMP0),			// TMP0 := TMP0 - 1
			ADD_DSZ64_DRI(R64SRC, R64SRC, 1),		// TODO hardcode RCX, any difference?
			ADD_DSZ64_DRI(R64SRC, R64SRC, 1),		// TODO operate on uarch registers, do only one write to R64SRC
			NOP_SEQWORD,
		}, {
			ADD_DSZ64_DRI(R64SRC, R64SRC, 1),
			UJMPCC_DIRECT_NOTTAKEN_CONDNZ_RI(TMP0, (PATCH_ADDR + 0x04)),
			NOP,
			// MOVETOCREG_DSZ64_RI(R12, 0x38c),		// Restore frontend (either this or the NOP above)
			( SEQ_UEND0(2) | SEQ_NEXT | SEQ_SYNCFULL(1) ),
			// If I change to SYNCFULL(2) in order to move the jump one uinstr below,
			// even without moving the jump itself one step down, the cpu just dies lol.
			// Not gonna bother with that.
			// Maybe UEND and SYNC can't be on the same uinstr? Kinda makes sense.
		}
	};

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
		putu32(uart_base, output); // 0x000DFFFF*3=0x0029FFFD
		// Careful with sending too many bytes in a row or the fifo will fill up
	}
}
