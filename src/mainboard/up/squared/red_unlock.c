/*
 * Federico (ceres-c) Cerutti
*/
#pragma GCC push_options
#pragma GCC optimize ("O0")

#include "red_unlock.h"

/* Pick the target here */
// #include "targets/mul.h"
// #include "targets/load.h"
// #include "targets/cmp.h"
// #include "targets/reg.h"
// #include "targets/ucode_update.h"	/* See note in this file for usage instructions */
// #include "targets/rdrand_1337.h"
// #include "targets/rdrand_cmp_ne.h"
// #include "targets/rdrand_cmp_ne_jmp.h"
// #include "targets/rdrand_sub_add.h"
// #include "targets/rdrand_add.h"
// #include "targets/rdrand_add_many.h"
// #include "targets/rdrand_move_regs.h"
// #include "targets/rdrand_or_regs.h"
// #include "targets/rdrand_jmp.h"
// #include "targets/rdrand_loop_add.h"
// #include "targets/rdrand_uram.h"
#include "targets/rdrand_uram_cmp_set.h"

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

	void* uart_base = uart_platform_baseptr(CONFIG(UART_FOR_CONSOLE));

	/*
	 * Will send to the glitcher 2 main commands/responses:
	 * - T_CMD_READY:	Ready to mark liveness and trigger the glitch
	 * - T_CMD_DONE:	Done with this loop iteration
	 * 					Will then send some more uint32_t's
	 *					(e.g. iterations, result_a, result_b...)
	 *
	 * NOTE: The code that is target of the glitch is repeated enough times to make
	 *		 each iteration last at least ~420 us. This is to give enough time to the
	 *		 PMIC to drop the voltage to Vp first and Vf later, and then to recover
	 *		 to Vcc before we are sending data over UART.
	 */
	target_loop(uart_base);
	__builtin_unreachable();
}

#pragma GCC pop_options
