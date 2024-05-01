/*
 * Federico (ceres-c) Cerutti
*/

#pragma GCC push_options
#pragma GCC optimize ("O0")

#include <soc/ramstage.h>
#include <console/console.h>
#include <console/uart.h>
#include <console/uart8250mem.h>
#include <cpu/x86/msr.h>
#include <arch/cpuid.h>

#include "lib-micro-x86/misc.h"
#include "lib-micro-x86/ucode_macro.h"
#include "lib-micro-x86/udbg.h"
#include "lib-micro-x86/opcode.h"
#include "lib-micro-x86/match_and_patch_hook.h"

#define MAGIC_UNLOCK 0x200
#define UART_TIMEOUT 5000
// #define PRINT_CLOCK_SPEED // Decomment if you want to enable clock speed printing at boot (BIOS_INFO log level)

/* Make coreboot old-ass gcc happy */
uint32_t ucode_addr_to_patch_addr(uint32_t addr);
uint32_t ucode_addr_to_patch_seqword_addr(uint32_t addr);
void ldat_array_write(uint32_t pdat_reg, uint32_t array_sel, uint32_t bank_sel, uint32_t dword_idx, uint32_t fast_addr, uint64_t val);
void ms_array_write(uint32_t array_sel, uint32_t bank_sel, uint32_t dword_idx, uint32_t fast_addr, uint64_t val);
static void ms_array_4_write(uint32_t addr, uint64_t val);
static void ms_array_2_write(uint32_t addr, uint64_t val);
void patch_ucode(uint32_t addr, ucode_t ucode_patch[], int n);
void hook_match_and_patch(uint32_t entry_idx, uint32_t ucode_addr, uint32_t patch_addr);
void do_fix_IN_patch(void);
void do_rdrand_patch(void);
#ifdef PRINT_CLOCK_SPEED
static unsigned long cpu_max_khz_from_cpuid(void);
static unsigned long curr_clock_khz(void);
#endif

uint32_t ucode_addr_to_patch_addr(uint32_t addr) {
    return addr - 0x7c00;
}
uint32_t ucode_addr_to_patch_seqword_addr(uint32_t addr) {
    uint32_t base = addr - 0x7c00;
    uint32_t seq_addr = ((base%4) * 0x80 + (base/4));
    return seq_addr % 0x80;
}
void ldat_array_write(uint32_t pdat_reg, uint32_t array_sel, uint32_t bank_sel, uint32_t dword_idx, uint32_t fast_addr, uint64_t val) {
    uint64_t prev = crbus_read(0x692);
    crbus_write(0x692, prev | 1);

    crbus_write(pdat_reg + 1, 0x30000 | ((dword_idx & 0xf) << 12) | ((array_sel & 0xf) << 8) | (bank_sel & 0xf));
    crbus_write(pdat_reg, 0x000000 | (fast_addr & 0xffff));
    crbus_write(pdat_reg + 4, val & 0xffffffff);
    crbus_write(pdat_reg + 5, (val >> 32) & 0xffff);
    crbus_write(pdat_reg + 1, 0);

    crbus_write(0x692, prev);
}
void ms_array_write(uint32_t array_sel, uint32_t bank_sel, uint32_t dword_idx, uint32_t fast_addr, uint64_t val) {
    ldat_array_write(0x6a0, array_sel, bank_sel, dword_idx, fast_addr, val);
}
/**
 * write a single microcode instruction to ms_array 4.
 * @param addr: The address to write to.
 * @param val: microcode instruction to write as a uint64_t.
 */
static void ms_array_4_write(uint32_t addr, uint64_t val) {return ms_array_write(4, 0, 0, addr, val); }
/**
 * write a single microcode instruction to ms_array 2.
 * @param addr: The address to write to.
 * @param val: microcode instruction to write as a uint64_t.
 */
static void ms_array_2_write(uint32_t addr, uint64_t val) {return ms_array_write(2, 0, 0, addr, val); }

void patch_ucode(uint32_t addr, ucode_t ucode_patch[], int n) {
    // format: uop0, uop1, uop2, seqword
    // uop3 is fixed to a nop and cannot be overridden
    for (int i = 0; i < n; i++) {
        // patch ucode
        ms_array_4_write(ucode_addr_to_patch_addr(addr + i*4)+0, CRC_UOP(ucode_patch[i].uop0));
        ms_array_4_write(ucode_addr_to_patch_addr(addr + i*4)+1, CRC_UOP(ucode_patch[i].uop1));
        ms_array_4_write(ucode_addr_to_patch_addr(addr + i*4)+2, CRC_UOP(ucode_patch[i].uop2));
        // patch seqword
        ms_array_2_write(ucode_addr_to_patch_seqword_addr(addr) + i, CRC_SEQ(ucode_patch[i].seqw));
    }
}

void hook_match_and_patch(uint32_t entry_idx, uint32_t ucode_addr, uint32_t patch_addr) {
	if (ucode_addr % 2 != 0) {
		die("[-] uop address must be even\n");
	}
	if (patch_addr % 2 != 0) {
		die("[-] patch uop address must be even\n");
	}

	uint32_t dst = patch_addr / 2;
	uint32_t patch_value = (dst << 16) | ucode_addr | 1;

	patch_ucode(match_and_patch_hook_addr, match_and_patch_hook_ucode_patch, ARRAY_SZ(match_and_patch_hook_ucode_patch));
	ucode_invoke_2(match_and_patch_hook_addr, patch_value, entry_idx<<1);
}

void do_fix_IN_patch(void) {
	// Patch U58ba to U017a
	hook_match_and_patch(0x1f, 0x58ba, 0x017a);
}

void do_rdrand_patch(void) {
	uint64_t patch_addr = 0x7da0;
	ucode_t ucode_patch[] = {
		/* Manipulate rax */
		{
			ZEROEXT_DSZ64_DI(RAX, 0x1337), /* Write zero extended */
			NOP,
			NOP,
			END_SEQWORD
		}
	};

	patch_ucode(patch_addr, ucode_patch, ARRAY_SZ(ucode_patch));
	hook_match_and_patch(0, RDRAND_XLAT, patch_addr);
}

inline static void putu32(void *uart_base, uint32_t d) {
	uart8250_mem_tx_byte(uart_base, d & 0xFF);
	uart8250_mem_tx_byte(uart_base, (d >> 8) & 0xFF);
	uart8250_mem_tx_byte(uart_base, (d >> 16) & 0xFF);
	uart8250_mem_tx_byte(uart_base, (d >> 24) & 0xFF);
	uart8250_mem_tx_flush(uart_base);
}

#ifdef PRINT_CLOCK_SPEED
static unsigned long cpu_max_khz_from_cpuid(void)
{
	/* Atom SoCs don't report the crystal frequency via CPUID, but they require a 25 MHz crystal.
	 * See Intel Atom® Processor C3000 Product Family Datasheet § 7.1 (Table 7-1, CLK_X1_PAD).
	 */
	unsigned int crystal_khz = 25000;

	/* Get numerator and denominator of TSC/crystal clock ratio.
	 * See Intel Atom® Processor C3000 Product Family Datasheet § 2.2.7.22 (Table 2-15).
	*/
	struct cpuid_result res = cpuid(0x15);
	uint32_t denominator = res.eax, numerator = res.ebx;
	return crystal_khz * numerator / denominator;
}

static unsigned long curr_clock_khz(void) {
	uint64_t aperf, mperf;
	uint32_t eax, edx;
	__asm__ volatile (
		"rdmsr"
		: "=a" (eax), "=d" (edx)
		: "c" (0xe7)
	);
	aperf = ((uint64_t)edx << 32) | eax;

	__asm__ volatile (
		"rdmsr"
		: "=a" (eax), "=d" (edx)
		: "c" (0xe8)
	);
	mperf = ((uint64_t)edx << 32) | eax;

	return cpu_max_khz_from_cpuid() * aperf / mperf;
}
#endif

void red_unlock_payload(void)
{
	/* NOTE: I don't do any exception handling, that's why IDT_IN_EVERY_STAGE is enabled with RED_UNLOCK
	* If any weird exception is thrown, check that your system is actually red unlocked (right blob?)
	*/

	#ifdef PRINT_CLOCK_SPEED
	// NOTE1: This will not be printed if you don't set a high enough log level in your config
	// NOTE2: This will not play well with the glitcher as it does not expect this data to be printed. Disable
	printk(BIOS_EMERG, "Current clock: %ld kHz\n", curr_clock_khz());
	#endif

	/* Enable ucode debug */
	unsigned int low = 0, high = 0;
	__asm__ volatile ("wrmsr" : : "a" (MAGIC_UNLOCK), "d" (0), "c" (APL_UCODE_CRBUS_UNLOCK));
	__asm__ volatile ("rdmsr" : "=a" (low), "=d" (high) : "c" (APL_UCODE_CRBUS_UNLOCK));
	// printk(BIOS_INFO, "MSR value: 0x%x%x\n", high, low);
	if (high != 0 || low != MAGIC_UNLOCK) {
		die("\tFailed to write magic CRBUS MSR\n");
	}

	do_fix_IN_patch();
	do_rdrand_patch();
	// printk(BIOS_INFO, "RDRAND patched\n");

	void* uart_base = uart_platform_baseptr(CONFIG(UART_FOR_CONSOLE));
	while (true) {
		uart8250_mem_tx_byte(uart_base, 'R');			/* Ready - 0b01010010 */
		uart8250_mem_tx_flush(uart_base);

		int i;
		for (i = 0; !uart8250_mem_can_rx_byte(uart_base) && i < UART_TIMEOUT; i++) {
			__asm__ volatile ("nop");
		}
		if (i == UART_TIMEOUT) {
			continue;
		}

		if (uart8250_mem_rx_byte(uart_base) == 'C') {	/* Connected */
			uart8250_mem_tx_byte(uart_base, 'T');		/* Trigger here - 0b01010100 */
			uart8250_mem_tx_flush(uart_base);
			volatile uint32_t ret;
			register uint32_t eax asm("eax");
			__asm__ __volatile__(
				".byte 0x0f, 0xc7, 0xf0;"); /* rdrand eax - otherwise gcc gives `operand size mismatch` */
			ret = eax;
			uart8250_mem_tx_byte(uart_base, 'A');		/* Alive - canary for checking if the board is still on */
			putu32(uart_base, ret);
		}
		uart8250_mem_rx_flush(uart_base);
	}
}

#pragma GCC pop_options
