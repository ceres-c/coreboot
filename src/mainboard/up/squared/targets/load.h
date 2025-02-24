#include "targets.h"

#define CODE_BODY_LOAD \
"xor %%ecx, %%ecx;\n\t" \
"movl %[stack_storage], %%ebx;\n\t" \
"cmp %%eax, %%ebx;\n\t" \
"setne %%cl;\n\t" \
"cmovne %%ebx, %[wrong_value];\n\t" \
"addl %%ecx, %[fault_count];\n\t" \

inline static __attribute__((always_inline)) void target_loop(void* uart_base) {
	/* NOTE: One iteration of this actually takes ~600 us */
	while (1) {
		uart8250_mem_tx_byte(uart_base, T_CMD_READY);
		uart8250_mem_tx_flush(uart_base);

		uint32_t stack_storage = 0xAAAAAAAA; // 10101010...
		uint32_t fault_count = 0, wrong_value = 0;

		// AT&T syntax
		__asm__ volatile (
		"movl %[stack_storage], %%eax;\n\t"

		REP100(REP100(CODE_BODY_LOAD)) // 70k iterations
		REP100(REP100(CODE_BODY_LOAD)) // WTF if I have 60k repetitions instead of 70k, it takes 1/5 the time?
		REP100(REP100(CODE_BODY_LOAD))
		REP100(REP100(CODE_BODY_LOAD))
		REP100(REP100(CODE_BODY_LOAD))
		REP100(REP100(CODE_BODY_LOAD))
		REP100(REP100(CODE_BODY_LOAD))

		: [fault_count]	"+r" (fault_count),
		[wrong_value]	"+r" (wrong_value)
		: [stack_storage]	"m" (stack_storage)
		: "%eax",	// copy of stack_storage reference
		"%ebx",	// copy of stack_storage round
		"%ecx"	// scratch
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, fault_count);
		putu32(uart_base, wrong_value);
		// Careful with sending too many bytes in a row or the fifo will fill up
	}
}
