#include "targets.h"

#define CODE_BODY_REG \
"movb %%al, %%bl;\n\t" \
"add %%ebx, %%ecx;\n\t"

inline static __attribute__((always_inline)) void target_loop(void* uart_base) {
	uint32_t summation = 0;

	// AT&T syntax
	__asm__ volatile (
	"xor %%ecx, %%ecx;\n\t"
	"xor %%ebx, %%ebx;\n\t"
	"mov $0x0101, %%eax;\n\t"

	REP10(REP100(REP100(CODE_BODY_REG))) // 271k iterations
	REP10(REP100(REP100(CODE_BODY_REG)))
	REP100(REP100(CODE_BODY_REG))
	REP100(REP100(CODE_BODY_REG))
	REP100(REP100(CODE_BODY_REG))
	REP100(REP100(CODE_BODY_REG))
	REP100(REP100(CODE_BODY_REG))
	REP100(REP100(CODE_BODY_REG))
	REP100(REP100(CODE_BODY_REG))
	REP10(REP100(CODE_BODY_REG))

	: "+c" (summation)
	:
	: "%eax"
	);

	uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
	putu32(uart_base, summation);
	// Careful with sending too many bytes in a row or the fifo will fill up
}
