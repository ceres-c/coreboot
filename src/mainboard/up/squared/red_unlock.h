#ifndef RED_UNLOCK_H
#define RED_UNLOCK_H

#include <soc/ramstage.h>
#include <console/console.h>
#include <console/uart.h>
#include <console/uart8250mem.h>
#include <arch/cpuid.h>
#include <cpu/intel/microcode.h>

#include "lib-micro-x86/lib-micro-minimal.h"

#define T_CMD_READY					'R'		/* Signal loop iteration start/trigger reference	*/
#define T_CMD_DONE					'D'		/* Done with the current loop iteration				*/

// #define PRINT_CLOCK_SPEED				/* Decomment if you want to print clock speed at boot (BIOS_INFO log level) */
// #define PRINT_UCODE_REV					/* Decomment if you want to print microcode revision at boot (BIOS_INFO log level) */

inline static __attribute__((always_inline)) void putu32(void *uart_base, uint32_t d) {
	uart8250_mem_tx_byte(uart_base, d & 0xFF);
	uart8250_mem_tx_byte(uart_base, (d >> 8) & 0xFF);
	uart8250_mem_tx_byte(uart_base, (d >> 16) & 0xFF);
	uart8250_mem_tx_byte(uart_base, (d >> 24) & 0xFF);
}

#ifdef PRINT_CLOCK_SPEED
static unsigned long cpu_max_khz_from_cpuid(void);
static unsigned long curr_clock_khz(void);
#endif

#endif
