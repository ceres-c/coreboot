/*
 * Federico (ceres-c) Cerutti
*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"	/* You're not my real dad, GCC. Let me live */
#pragma GCC diagnostic ignored "-Wunused-function"

#pragma GCC push_options
#pragma GCC optimize ("O0")

#include <timestamp.h>
#include <soc/ramstage.h>
#include <console/console.h>
#include <console/uart.h>
#include <console/uart8250mem.h>
#include <cpu/x86/msr.h>
#include <arch/cpuid.h>
#include <cpu/intel/microcode.h>

#include "lib-micro-x86/misc.h"
#include "lib-micro-x86/ucode_macro.h"
#include "lib-micro-x86/udbg.h"
#include "lib-micro-x86/opcode.h"
#include "lib-micro-x86/match_and_patch_hook.h"

#define T_CMD_READY					'R'		/* Signal loop iteration start/trigger reference	*/
#define T_CMD_DONE					'D'		/* Done with the current loop iteration				*/
#define REP10(BODY) BODY BODY BODY BODY BODY BODY BODY BODY BODY BODY
#define REP100(BODY) REP10(REP10(BODY))

#define MAGIC_UNLOCK 0x200
// #define PRINT_CLOCK_SPEED				/* Decomment if you want to print clock speed at boot (BIOS_INFO log level) */
// #define PRINT_UCODE_REV					/* Decomment if you want to print microcode revision at boot (BIOS_INFO log level) */
// #define TARGET_MUL
// #define TARGET_LOAD
// #define TARGET_CMP
// #define TARGET_RDRAND_1337
// #define TARGET_RDRAND_CMP_NE
// #define TARGET_RDRAND_CMP_NE_JMP
// #define TARGET_RDRAND_SUB_ADD
// #define TARGET_RDRAND_ADD
// #define TARGET_RDRAND_ADD_MANY
// #define TARGET_RDRAND_MOVE_REGS
// #define TARGET_RDRAND_OR_REGS
#define TARGET_UCODE_UPDATE
#if (defined(TARGET_MUL) + defined(TARGET_LOAD) + defined(TARGET_CMP) +	\
	 defined(TARGET_RDRAND_1337) + \
	 defined(TARGET_RDRAND_CMP_NE) + defined(TARGET_RDRAND_CMP_NE_JMP) + \
	 defined(TARGET_RDRAND_SUB_ADD) + defined(TARGET_RDRAND_ADD) +		\
	 defined(TARGET_RDRAND_ADD_MANY) + defined(TARGET_RDRAND_MOVE_REGS)) + \
	 defined(TARGET_RDRAND_OR_REGS) + defined(TARGET_UCODE_UPDATE) != 1
#error You should pick exactly one glitch target
#endif

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
		#if defined(TARGET_RDRAND_1337) /* rcx = 0x1337 */
		{
			ZEROEXT_DSZ32_DI(RCX, 0x1337), /* Write zero extended */
			NOP,
			NOP,
			END_SEQWORD
		},
		#elif defined(TARGET_RDRAND_CMP_NE) /* rcx += rax != rbx (with conditional set) */
		{
			SUB_DSZ32_DRR(TMP0, RAX, RBX),	/* tmp0 = rax - rbx. tmp0 now has per-register flags set */
			SETCC_CONDNZ_DR(TMP1, TMP0),
			ADD_DSZ32_DRR(RCX, TMP1, RCX),	/* NOTE: Both ADD and SUB 64 bit opcodes work just as well */
			END_SEQWORD
		},
		#elif defined(TARGET_RDRAND_CMP_NE_JMP) /* rcx += rax != rbx (with jumps) */
		#error RDRAND_CMP with jumps does not work, currently. Jumps are broken for unknown reasons in coreboot.
		{
			SUB_DSZ32_DRR(TMP0, RAX, RBX),	/* tmp0 = rax - rbx. tmp0 now has per-register flags set */
			UJMPCC_DIRECT_NOTTAKEN_CONDNZ_RI(TMP0, patch_addr + 0x04), // NOTE: This is the jump that breaks everything. Works in linux
			NOP,
			END_SEQWORD
		},
		{
			ADD_DSZ32_DRI(RCX, RCX, 1),
			NOP,
			NOP,
			END_SEQWORD
		},
		#elif defined(TARGET_RDRAND_SUB_ADD)  /* rcx += rax - rbx (potentially huge jumps) */
		{
			SUB_DSZ64_DRR(TMP0, RAX, RBX),	/* tmp0 = rax - rbx. tmp0 now has per-register flags set */
			ADD_DSZ64_DRR(RCX, TMP0, RCX),
			NOP,
			END_SEQWORD
		},
		#elif defined(TARGET_RDRAND_ADD) /* rcx += 1 */
		{
			ADD_DSZ64_DRI(RCX, RCX, 1),
			NOP,
			NOP,
			END_SEQWORD
		},
		#elif defined(TARGET_RDRAND_ADD_MANY)  /* rcx += 10 - Do +1 10 times in a row */
		/* This is useful to detect whether I am skipping architectural op 'rdrand' or microarchitectural uop 'add' */
		{
			ADD_DSZ64_DRI(RCX, RCX, 1),
			ADD_DSZ64_DRI(RCX, RCX, 1),
			ADD_DSZ64_DRI(RCX, RCX, 1),
			NOP_SEQWORD
		},
		{
			ADD_DSZ64_DRI(RCX, RCX, 1),
			ADD_DSZ64_DRI(RCX, RCX, 1),
			ADD_DSZ64_DRI(RCX, RCX, 1),
			NOP_SEQWORD
		},
		{
			ADD_DSZ64_DRI(RCX, RCX, 1),
			ADD_DSZ64_DRI(RCX, RCX, 1),
			ADD_DSZ64_DRI(RCX, RCX, 1),
			NOP_SEQWORD
		},
		{
			ADD_DSZ64_DRI(RCX, RCX, 1),
			NOP,
			NOP,
			END_SEQWORD
		},
		#elif defined(TARGET_RDRAND_XOR) /* rcx |= rax ^ rbx */
		{
			XOR_DSZ64_DRR(TMP0, RAX, RBX),	/* tmp0 = rax ^ rbx */
			OR_DSZ64_DRR(RCX, TMP0, RCX),
			NOP,
			END_SEQWORD
		},
		#elif defined(TARGET_RDRAND_MOVE_REGS) /* rcx = rcx */
		{ /* Move around the current value of rcx without explicitly changing it, then store back to rcx. Test issues with register file */
			ZEROEXT_DSZ32_DR(TMP0, RCX),
			ZEROEXT_DSZ32_DR(TMP1, TMP0),
			ZEROEXT_DSZ32_DR(TMP2, TMP1),
			NOP_SEQWORD
		},
		{
			ZEROEXT_DSZ32_DR(TMP3, TMP2),
			ZEROEXT_DSZ32_DR(TMP4, TMP3),
			ZEROEXT_DSZ32_DR(TMP5, TMP4),
			NOP_SEQWORD
		},
		{
			ZEROEXT_DSZ32_DR(TMP6, TMP5),
			ZEROEXT_DSZ32_DR(TMP7, TMP6),
			ZEROEXT_DSZ32_DR(TMP8, TMP7),
			NOP_SEQWORD
		},
		{
			ZEROEXT_DSZ32_DR(TMP9, TMP8),
			ZEROEXT_DSZ32_DR(TMP10, TMP9),
			ZEROEXT_DSZ32_DR(RCX, TMP10),
			END_SEQWORD
		},
		#elif defined(TARGET_RDRAND_OR_REGS) /* rcx = rcx */
		{ /* Move around the current value of rcx without explicitly changing it, then store back to rcx. Test issues with register file */
			OR_DSZ32_DRI(TMP0, RCX, 0),
			OR_DSZ32_DRI(TMP1, TMP0, 0),
			OR_DSZ32_DRI(TMP2, TMP1, 0),
			NOP_SEQWORD
		},
		{
			OR_DSZ32_DRI(TMP3, TMP2, 0),
			OR_DSZ32_DRI(TMP4, TMP3, 0),
			OR_DSZ32_DRI(TMP5, TMP4, 0),
			NOP_SEQWORD
		},
		{
			OR_DSZ32_DRI(TMP6, TMP5, 0),
			OR_DSZ32_DRI(TMP7, TMP6, 0),
			OR_DSZ32_DRI(TMP8, TMP7, 0),
			NOP_SEQWORD
		},
		{
			OR_DSZ32_DRI(TMP9, TMP8, 0),
			OR_DSZ32_DRI(TMP10, TMP9, 0),
			OR_DSZ32_DRI(RCX, TMP10, 0),
			END_SEQWORD
		},
		#endif
	};

	#if (defined(TARGET_RDRAND_1337) + \
		 defined(TARGET_RDRAND_CMP_NE) + defined(TARGET_RDRAND_CMP_NE_JMP) + \
		 defined(TARGET_RDRAND_SUB_ADD) + defined(TARGET_RDRAND_ADD) +		\
		 defined(TARGET_RDRAND_ADD_MANY) + defined(TARGET_RDRAND_MOVE_REGS)) + \
		 defined(TARGET_RDRAND_OR_REGS) == 1
	patch_ucode(patch_addr, ucode_patch, ARRAY_SZ(ucode_patch));
	hook_match_and_patch(0, RDRAND_XLAT, patch_addr);
	printk(BIOS_INFO, "RDRAND patched\n");
	#endif
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
	/* If invalid instruction/any weird exception is thrown, check that your system is actually red unlocked (right blob?) */

	/* Print clock speed, if needed for reporting/debugging
	 * NOTE: This is not compatible with the glitcher as it does not expect this data to be printed.
	 */
	#ifdef PRINT_CLOCK_SPEED
	printk(BIOS_INFO, "Current clock: %ld kHz\n", curr_clock_khz());
	#endif

	#ifdef PRINT_UCODE_REV
	/* Print ucode revision as it is running (comes from FIT package) */
	uint32_t print_ucode_rev = read_microcode_rev(); // 0x20
	printk(BIOS_INFO, "Microcode FIT revision: 0x%x\n", print_ucode_rev);

	/* No need for spinlocks here, I assume. We are still running a single core */
	const void *print_patch = intel_microcode_find();
	const struct microcode *ucode_print_patch = print_patch;
	if (!ucode_print_patch)
		die("microcode: failed because no ucode was found\n");
	if (print_ucode_rev == ucode_print_patch->rev)
		die("microcode: Update skipped, already up-to-date\n");
	unsigned long ucode_print_patch_addr = (unsigned long)ucode_print_patch + sizeof(struct microcode);
	uint64_t ucode_print_tsc = timestamp_get();
	__asm__ __volatile__ (
		"wrmsr"
		: /* No outputs */
		: "c" (IA32_BIOS_UPDT_TRIG), "a" (ucode_print_patch_addr), "d" (0)
	);
	uint64_t ucode_print_tsc2 = timestamp_get();
	print_ucode_rev = read_microcode_rev();
	printk(BIOS_INFO, "Microcode CBFS revision: 0x%x\n", print_ucode_rev);
	printk(BIOS_INFO, "Microcode update took %lld cycles\n", ucode_print_tsc2 - ucode_print_tsc);
	#endif /* PRINT_UCODE_REV */

	/* Enable ucode debug */
	unsigned int low = 0, high = 0;
	__asm__ volatile ("wrmsr" : : "a" (MAGIC_UNLOCK), "d" (0), "c" (APL_UCODE_CRBUS_UNLOCK));
	__asm__ volatile ("rdmsr" : "=a" (low), "=d" (high) : "c" (APL_UCODE_CRBUS_UNLOCK));
	if (high != 0 || low != MAGIC_UNLOCK) {
		die("\tFailed to write APL_UCODE_CRBUS_UNLOCK MSR\n");
	}

	/* Patch IN instruction and apply our patch to rdrand */
	do_fix_IN_patch(); /* See 'Backdoor in the Core' talk to see why this is needed */
	do_rdrand_patch();

	void* uart_base = uart_platform_baseptr(CONFIG(UART_FOR_CONSOLE));

	#ifdef TARGET_UCODE_UPDATE
	uint32_t ucode_rev = read_microcode_rev(); // 0x20
	const void *patch = intel_microcode_find();
	const struct microcode *ucode_patch = patch;
	if (!ucode_patch)
		die("microcode: failed because no ucode was found\n");
	if (ucode_rev == ucode_patch->rev)
		die("microcode: failed because already up-to-date\n");
	unsigned long ucode_patch_addr = (unsigned long)ucode_patch + sizeof(struct microcode);
	#endif /* TARGET_UCODE_UPDATE */

	while (true) {
		/*
		 * Will send to the glitcher 2 main commands/responses:
		 * - T_CMD_READY:	Ready to mark liveness and trigger the glitch
		 * - T_CMD_DONE:	Done with this loop iteration
		 * 					Will then send some more uint32_t's (e.g. iterations, result_a, result_b...)
		 */
		uart8250_mem_tx_byte(uart_base, T_CMD_READY);
		uart8250_mem_tx_flush(uart_base);

		/*
		 * NOTE: The code that is target of the glitch is repeated enough times to make each iteration last
		 * ~420 us. This is to give enough time to the PMIC to drop the voltage to Vp first and Vf later,
		 * and then to recover to Vcc before we are sending data over UART.
		 */
		#if defined(TARGET_MUL)
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
			REP100(REP100(CODE_BODY_MUL)) // 54k iterations
			REP100(REP100(CODE_BODY_MUL))
			REP100(REP100(CODE_BODY_MUL))
			REP100(REP100(CODE_BODY_MUL))
			REP100(REP100(CODE_BODY_MUL))
			REP10(REP100(CODE_BODY_MUL))
			REP10(REP100(CODE_BODY_MUL))
			REP10(REP100(CODE_BODY_MUL))
			REP10(REP100(CODE_BODY_MUL))

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
		#elif defined(TARGET_LOAD) /* NOTE: One iteration of this actually takes ~600 us */
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
		#elif defined(TARGET_CMP)
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

			REP10(REP100(REP100(CODE_BODY_CMP))) // 110k iterations
			REP100(REP100(CODE_BODY_CMP))

			: [fault_count]		"+r" (fault_count)
			: [stack_storage]	"m" (stack_storage)
			: "%eax",	// Copy a of stack_storage
			  "%ebx",	// Copy b of stack_storage
			  "%ecx"	// Scratch
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, fault_count);
		// Careful with sending too many bytes in a row or the fifo will fill up
		#elif defined(TARGET_RDRAND_1337)
		#define CODE_BODY_RDRAND_1337 \
			"rdrand %%ecx;\t\n"

		uint32_t result = 0;

		// AT&T syntax
		__asm__ __volatile__ (
			"xor %%ecx, %%ecx\t\n"
			REP10(REP100(REP100(CODE_BODY_RDRAND_1337))) // 120k iterations
			REP100(REP100(CODE_BODY_RDRAND_1337))
			REP100(REP100(CODE_BODY_RDRAND_1337))
			: "=c" (result)									// Output operands
			:												// Input operands
			:												// Clobbered register
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, result);
		// Careful with sending too many bytes in a row or the fifo will fill up
		#elif defined(TARGET_RDRAND_CMP_NE) || defined(TARGET_RDRAND_CMP_NE_JMP)
		#define CODE_BODY_RDRAND_CMP_NE \
			"rdrand %%ecx;\t\n"

		uint32_t operand1 = 0b01, operand2 = 0b01; // Can really be anything, as long as they're equal, I guess?
		uint32_t diff_count = 0;

		#ifdef TARGET_RDRAND_CMP_NE_JMP
		#error Please check if 120k iterations corresponds to a ~400 us execution time for TARGET_RDRAND_CMP_NE_JMP
		#endif
		// AT&T syntax
		__asm__ __volatile__ (
			"xor %%ecx, %%ecx\t\n"
			REP10(REP100(REP100(CODE_BODY_RDRAND_CMP_NE))) // 120k iterations
			REP100(REP100(CODE_BODY_RDRAND_CMP_NE))
			REP100(REP100(CODE_BODY_RDRAND_CMP_NE))
			: "=c" (diff_count)								// Output operands
			: "a" (operand1),								// Input operands
			  "b" (operand2)
			: 												// Clobbered register
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, diff_count);
		// Careful with sending too many bytes in a row or the fifo will fill up
		#elif defined(TARGET_RDRAND_SUB_ADD)
		#define CODE_BODY_RDRAND_SUB_ADD \
			"rdrand %%ecx;\t\n"

		uint32_t operand1 = 0b01, operand2 = 0b01; // Can really be anything, as long as they're equal, I guess?
		uint32_t fault_count = 0;

		// AT&T syntax
		__asm__ __volatile__ (
			"xor %%ecx, %%ecx\t\n"
			REP10(REP100(REP100(CODE_BODY_RDRAND_SUB_ADD))) // 120k iterations
			REP100(REP100(CODE_BODY_RDRAND_SUB_ADD))
			REP100(REP100(CODE_BODY_RDRAND_SUB_ADD))
			: "=c" (fault_count)							// Output operands
			: "a" (operand1),								// Input operands
			  "b" (operand2)
			:												// Clobbered register
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, fault_count);
		// Careful with sending too many bytes in a row or the fifo will fill up
		#elif defined(TARGET_RDRAND_ADD) || defined(TARGET_RDRAND_ADD_MANY)
		#define CODE_BODY_RDRAND_ADD \
			"rdrand %%ecx;\t\n"

		uint32_t summation = 0;

		// AT&T syntax
		__asm__ __volatile__ (
			"xor %%ecx, %%ecx\t\n"

			#if defined(TARGET_RDRAND_ADD)
			REP10(REP100(REP100(CODE_BODY_RDRAND_ADD))) // 120k iterations
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			#elif defined(TARGET_RDRAND_ADD_MANY)
			REP100(REP100(CODE_BODY_RDRAND_ADD)) // 90k iterations (*10 adds per rdrand invoke) - ecx = 900000
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			REP100(REP100(CODE_BODY_RDRAND_ADD))
			#endif
			: "=c" (summation)								// Output operands
			:												// Input operands
			:												// Clobbered register
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, summation);
		// Careful with sending too many bytes in a row or the fifo will fill up
		#elif defined(TARGET_RDRAND_MOVE_REGS) || defined (TARGET_RDRAND_OR_REGS)
		#define CODE_BODY_RDRAND_MOVE_REGS \
			"rdrand %%ecx;\t\n"

		uint32_t input = 0xFFFFFFFF, output = 0;

		// AT&T syntax
		__asm__ __volatile__ (
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))		// 90k iterations
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			REP100(REP100(CODE_BODY_RDRAND_MOVE_REGS))
			: "=c" (output)									// Output operands
			: "c" (input)									// Input operands
			:												// Clobbered register
		);

		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, output);
		// Careful with sending too many bytes in a row or the fifo will fill up
		#elif defined(TARGET_UCODE_UPDATE) /* NOTE: One iteration of this actually takes ~5.55 ms (MILLI!) */
		/* To be more precise, it's ~5.27 ms when performing an update on top of the same update with a valid RSA
		 * signature, and ~5.18 ms when performing an update on top of the same update when the signature check fails.
		 */
		#define CODE_BODY_UCODE_UPDATE_DELAY \
			"nop;\t\n"
		__asm__ __volatile__ (
			"wrmsr;\t\n"
			REP10(REP100(REP100(CODE_BODY_UCODE_UPDATE_DELAY))) // This is the extra 300us delay, added to allow
			REP10(REP100(REP100(CODE_BODY_UCODE_UPDATE_DELAY))) // the glitcher to restore voltage before UART tx
			REP10(REP100(REP100(CODE_BODY_UCODE_UPDATE_DELAY)))
			REP10(REP100(REP100(CODE_BODY_UCODE_UPDATE_DELAY)))
			REP10(REP100(REP100(CODE_BODY_UCODE_UPDATE_DELAY)))
			: /* No outputs */
			: "c" (IA32_BIOS_UPDT_TRIG), "a" (ucode_patch_addr), "d" (0)
		);

		uint32_t updated_ucode_rev = read_microcode_rev();
		uart8250_mem_tx_byte(uart_base, T_CMD_DONE);
		putu32(uart_base, updated_ucode_rev);
		#endif
	}
}

#pragma GCC pop_options
#pragma GCC diagnostic pop
