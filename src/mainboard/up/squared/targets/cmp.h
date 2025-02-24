#include "targets.h"

#define CODE_BODY_CMP \
"cmp %%eax, %%ebx;\n\t" \
"setne %%dl;\n\t" \
"addb %%dl, %%cl;\n\t" // Might actually overflow at 256 and wrap around, but heh

inline static __attribute__((always_inline)) void target_loop(void* uart_base) {
	while (1) {
		uart8250_mem_tx_byte(uart_base, T_CMD_READY);
		uart8250_mem_tx_flush(uart_base);

		uint32_t source_value = 0xAAAAAAAA; // 10101010...
		uint32_t fault_count = 0;

		// AT&T syntax
		__asm__ volatile (
		"xor %%edx, %%edx;\n\t"

		REP10(REP100(REP100(CODE_BODY_CMP))) // 150k iterations
		REP100(REP100(CODE_BODY_CMP))
		REP100(REP100(CODE_BODY_CMP))
		REP100(REP100(CODE_BODY_CMP))
		REP100(REP100(CODE_BODY_CMP))
		REP100(REP100(CODE_BODY_CMP))

		: "+c" (fault_count)
		: "a" (source_value),
		"b" (source_value)
		: "%edx"	// Scratch
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, fault_count);
		// Careful with sending too many bytes in a row or the fifo will fill up
	}
}
