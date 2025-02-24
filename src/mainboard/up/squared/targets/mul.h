#include "targets.h"

#define TARGET_NO_REDUNLOCK

#define CODE_BODY_MUL \
"movl %%eax, %%edx;\n\t" \
"imull %%ebx, %%edx;\n\t" \
"movl %%eax, %%edi;\n\t" \
"imull %%ebx, %%edi;\n\t" \
"cmp %%edx, %%edi;\n\t" \
"setne %%dl;\n\t" \
"addb %%dl, %%cl;\n\t" // Might actually overflow at 256 and wrap around, but heh

inline static __attribute__((always_inline)) void target_loop(void* uart_base) {
	while (1) {
		uart8250_mem_tx_byte(uart_base, T_CMD_READY);
		uart8250_mem_tx_flush(uart_base);

		// uint32_t operand1 = 0x80000, operand2 = 0x4; // Taken from plundervolt paper
		uint32_t operand1 = 0x4, operand2 = 0x80000; // Taken from plundervolt paper
		uint32_t fault_count = 0;

		// AT&T syntax
		__asm__ volatile (
		"xor %%ecx, %%ecx;\n\t" \

		REP100(REP100(CODE_BODY_MUL)) // 56k iterations
		REP100(REP100(CODE_BODY_MUL))
		REP100(REP100(CODE_BODY_MUL))
		REP100(REP100(CODE_BODY_MUL))
		REP100(REP100(CODE_BODY_MUL))
		REP10(REP100(CODE_BODY_MUL))
		REP10(REP100(CODE_BODY_MUL))
		REP10(REP100(CODE_BODY_MUL))
		REP10(REP100(CODE_BODY_MUL))
		REP10(REP100(CODE_BODY_MUL))
		REP10(REP100(CODE_BODY_MUL))

		: "+c" (fault_count)							// Output operands
		: "a" (operand1),								// Input operands
		"b" (operand2)
		: "%edx", // result_a							// Clobbered register
		"%edi"  // result_b
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, fault_count);
		// Careful with sending too many bytes in a row, or the UART FIFO (64 bytes) will fill up
	}
}
