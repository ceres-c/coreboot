/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h> // TODO remove if I get rid of printk
#include <fsp/util.h>
#include <arch/exception.h>
#include <cpu/x86/msr.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

volatile bool msr_exists;

static bool is_red_unlocked(void)
{
	/*
	 * Detect whether the CPU is already red unlocked.
	 */
	/* Install our exception handler */
	// idt[13].offset_0 = (uintptr_t)vec13_msr_handler;
	// idt[13].offset_1 = (uintptr_t)vec13_msr_handler >> 16;

	uint32_t low = 0, high = 0; // NOTE: These variables will be populated with garbage if the MSR doesn't exist
	msr_exists = true;
	__asm__ volatile ("rdmsr" : "=a" (low), "=d" (high) : "c" (APL_UCODE_CRBUS_UNLOCK));
	if (msr_exists) { // Exception handler sets this flag
		printk(BIOS_INFO, "[MSR] " STR(APL_UCODE_CRBUS_UNLOCK) " found! Red unlocked already :)\n");
	} else {
		printk(BIOS_INFO, "[MSR] " STR(APL_UCODE_CRBUS_UNLOCK) " doesn't exist :(\n");
	}

	/* Restore the original interrupt handler */
	idt[13].offset_0 = (uintptr_t)vec13;
	idt[13].offset_1 = (uintptr_t)vec13 >> 16;

	return msr_exists;
}

const char *soc_select_fsp_m_cbfs(void)
{
	return is_red_unlocked() ? CONFIG_FSP_M_CBFS : CONFIG_FSP_M_CBFS_2;
}

const char *soc_select_fsp_s_cbfs(void)
{
	return is_red_unlocked() ? CONFIG_FSP_S_CBFS : CONFIG_FSP_S_CBFS_2;
}
