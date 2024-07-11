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

#define REP10(BODY) BODY BODY BODY BODY BODY BODY BODY BODY BODY BODY
#define REP100(BODY) REP10(REP10(BODY))

// #define TARGET_MUL
// #define TARGET_LOAD
// #define TARGET_CMP
// #define TARGET_RDRAND_SUB_ADD
#define TARGET_RDRAND_ADD
#if (defined(TARGET_MUL) + defined(TARGET_LOAD) + defined(TARGET_CMP) + defined(TARGET_RDRAND_SUB_ADD) + defined(TARGET_RDRAND_ADD)) != 1
#error You should pick exactly one glitch target
#endif

#define T_CMD_READY					'R'
#define T_CMD_DONE					'D'		/* Done with the current loop iteration */

#define MAGIC_UNLOCK 0x200
// #define PRINT_CLOCK_SPEED	// Decomment if you want to enable clock speed printing at boot (BIOS_INFO log level)

#define MUL_ITERATIONS 100000

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
		#if defined(TARGET_RDRAND_1337)
		{ /* rcx = 0x1337 */
			ZEROEXT_DSZ64_DI(RCX, 0x1337), /* Write zero extended */
			NOP,
			NOP,
			END_SEQWORD
		},
		#elif defined(TARGET_RDRAND_CMP)
		/* This code with the conditional jump is not working in coreboot. Here, it hangs after a (small)
		 * number of iterations, but it was working in linux (?)
		 * idk...
		 */
		{ /* rcx += rax != rbx ? 1 : 0 */
			SUB_DSZ64_DRR(TMP0, RAX, RBX),	/* tmp0 = rax - rbx. tmp0 now has per-register flags set */
			UJMPCC_DIRECT_NOTTAKEN_CONDNZ_RI(TMP0, patch_addr + 0x04),
			NOP,
			END_SEQWORD
		},
		{ // 0x04
			MOVE_DSZ64_DR(TMP0, RCX),		/* Move current value of rcx to tmp0, because ucode ADD can */
			ADD_DSZ64_DRI(RCX, TMP0, 1),	/* operate on only one architectual register at a time, it seems */
			NOP,
			END_SEQWORD
		},
		#elif defined(TARGET_RDRAND_SUB_ADD)
		{ /* rcx += rax - rbx (potentially huge jumps) */
			SUB_DSZ64_DRR(TMP0, RAX, RBX),	/* tmp0 = rax - rbx. tmp0 now has per-register flags set */
			ADD_DSZ64_DRR(RCX, TMP0, RCX),
			NOP,
			END_SEQWORD
		},
		#elif defined(TARGET_RDRAND_ADD)
		{ /* rcx += 1 */
			ADD_DSZ64_DRI(RCX, RCX, 1),
			NOP,
			NOP,
			END_SEQWORD
		},
		#elif defined(TARGET_RDRAND_XOR)
		{ /* rcx |= rax ^ rbx */
			XOR_DSZ64_DRR(TMP0, RAX, RBX),	/* tmp0 = rax ^ rbx */
			OR_DSZ64_DRR(RCX, TMP0, RCX),
			NOP,
			END_SEQWORD
		},
		#endif
	};

	patch_ucode(patch_addr, ucode_patch, ARRAY_SZ(ucode_patch));
	hook_match_and_patch(0, RDRAND_XLAT, patch_addr);
}

inline static __attribute__((always_inline)) void putu32(void *uart_base, uint32_t d) {
	uart8250_mem_tx_byte(uart_base, d & 0xFF);
	uart8250_mem_tx_byte(uart_base, (d >> 8) & 0xFF);
	uart8250_mem_tx_byte(uart_base, (d >> 16) & 0xFF);
	uart8250_mem_tx_byte(uart_base, (d >> 24) & 0xFF);
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
	printk(BIOS_INFO, "RDRAND patched\n");

	void* uart_base = uart_platform_baseptr(CONFIG(UART_FOR_CONSOLE));
	while (true) {
		/*
		 * Will send to the glitcher 2 main commands/responses:
		 * - T_CMD_READY:	Ready to mark liveness and trigger the glitch
		 * - T_CMD_DONE:	Done with this loop iteration
		 * 					Will then send some more uint32_t's (e.g. iterations, result_a, result_b...)
		 */
		uart8250_mem_tx_byte(uart_base, T_CMD_READY);
		uart8250_mem_tx_flush(uart_base);

		// TODO cpuid, mfence, rdtsc, rdtscp, lfence, cpuid...

		#ifdef TARGET_MUL
		#define CODE_BODY_MUL \
			"xor %%ecx, %%ecx;\n\t" \
			"movl %[op1], %%eax;\n\t" \
			"imull %[op2], %%eax;\n\t" \
			"movl %[op1], %%ebx;\n\t" \
			"imull %[op2], %%ebx;\n\t" \
			"cmp %%eax, %%ebx;\n\t" \
			"setne %%cl;\n\t" \
			"addl %%ecx, %[fault_count];\n\t"

		uint32_t operand1 = 0x80000, operand2 = 0x4; // Taken from plundervolt paper
		uint32_t fault_count = 0, result_a = 0, result_b = 0;

		// AT&T syntax
		__asm__ volatile (
			REP100(REP100(CODE_BODY_MUL)) // 50k iterations
			REP100(REP100(CODE_BODY_MUL))
			REP100(REP100(CODE_BODY_MUL))
			REP100(REP100(CODE_BODY_MUL))
			REP100(REP100(CODE_BODY_MUL))

			: [fault_count]	"+r" (fault_count)				// Output operands
			: [op1]			"r" (operand1),					// Input operands
			  [op2]			"r" (operand2)
			: "%eax", // result_a							// Clobbered register
			  "%ebx", // result_b
			  "%ecx"  // scratch
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, fault_count);
		putu32(uart_base, result_a);
		putu32(uart_base, result_b);
		// Careful with sending too many bytes in a row, or the UART FIFO (64 bytes) will fill up
		#endif // TARGET_MUL

		#ifdef TARGET_LOAD
		#define CODE_BODY_LOAD \
			"xor %%ecx, %%ecx;\n\t" \
			"movl %[stack_storage], %%ebx;\n\t" \
			"cmp %%eax, %%ebx;\n\t" \
			"setne %%cl;\n\t" \
			"cmovne %%ebx, %[wrong_value];\n\t" \
			"addl %%ecx, %[fault_count];\n\t" \

		uint32_t stack_storage = 0xAAAAAAAA; // 10101010...
		uint32_t fault_count = 0, wrong_value = 0;

		// AT&T syntax
		__asm__ volatile (
			"movl %[stack_storage], %%eax;\n\t"

			REP100(REP100(CODE_BODY_LOAD)) // 70k iterations
			REP100(REP100(CODE_BODY_LOAD)) // WTF if I have 60k iterations instead of 70k, it takes 1/2 the time?
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
		#endif // TARGET_LOAD

		#ifdef TARGET_CMP
		#define CODE_BODY_CMP \
			"xor %%ecx, %%ecx;\n\t" \
			"cmp %%eax, %%ebx;\n\t" \
			"setne %%cl;\n\t" \
			"addl %%ecx, %[fault_count];\n\t"

		uint32_t stack_storage = 0xAAAAAAAA; // 10101010...
		uint32_t fault_count = 0;

		// AT&T syntax
		__asm__ volatile (
			"movl %[stack_storage], %%eax;\n\t"
			"movl %[stack_storage], %%ebx;\n\t"

			REP10(REP100(REP100(CODE_BODY_CMP))) // 100k iterations

			: [fault_count]		"+r" (fault_count)
			: [stack_storage]	"m" (stack_storage)
			: "%eax",	// Copy a of stack_storage
			  "%ebx",	// Copy b of stack_storage
			  "%ecx"	// Scratch
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, fault_count);
		// Careful with sending too many bytes in a row or the fifo will fill up
		#endif // TARGET_CMP

		#ifdef TARGET_RDRAND_SUB_ADD
		#define CODE_BODY_RDRAND_SUB_ADD \
			"rdrand %%ecx;\t\n"

		uint32_t operand1 = 0b01, operand2 = 0b01; // Can really be anything, as long as they're equal, I guess?
		uint32_t fault_count = 0;

		// AT&T syntax
		__asm__ __volatile__ (
			"xor %%ecx, %%ecx\t\n"
			"mov %[op1], %%eax;\t\n"
			"mov %[op2], %%ebx;\t\n"

			REP10(REP100(REP100(CODE_BODY_RDRAND_SUB_ADD))) // 120k iterations
			REP100(REP100(CODE_BODY_RDRAND_SUB_ADD))
			REP100(REP100(CODE_BODY_RDRAND_SUB_ADD))

			"mov %%ecx, %[fault_count];\t\n"
			: [fault_count]	"=r" (fault_count)				// Output operands
			: [op1]			"r" (operand1),					// Input operands
			  [op2]			"r" (operand2)
			: "%eax", // operand1							// Clobbered register
			  "%ebx", // operand2
			  "%ecx"  // result
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, fault_count);
		// Careful with sending too many bytes in a row or the fifo will fill up
		#endif // TARGET_RDRAND_SUB_ADD
		#ifdef TARGET_RDRAND_ADD
		#define CODE_BODY_RDRAND_ADD \
			"rdrand %%ecx;\t\n"

		uint32_t summation = 0;

		// AT&T syntax
		__asm__ __volatile__ (
			"xor %%ecx, %%ecx\t\n"

			REP10(REP100(REP100(CODE_BODY_RDRAND_ADD))) // 120k iterations
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			REP100(REP100(CODE_BODY_RDRAND_ADD))

			"mov %%ecx, %[summation];\t\n"
			: [summation]	"=r" (summation)				// Output operands
			:												// Input operands
			: "%ecx" // result								// Clobbered register
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, summation);
		// Careful with sending too many bytes in a row or the fifo will fill up
		#endif // TARGET_RDRAND_ADD
	}
}

#pragma GCC pop_options
