/*
 * Federico (ceres-c) Cerutti
*/

#include <bootblock_common.h>
#include <console/console.h>
#include <cpu/x86/msr.h>

void bootblock_red_unlock_payload(void)
{
	/* NOTE: I don't do any exception handling, that's why IDT_IN_EVERY_STAGE is enabled with RED_UNLOCK
	 * If any weird exception is thrown, check that your system is actually red unlocked (right blob?)
	 */
	unsigned int low = 0, high = 0;
	__asm__ volatile ("rdmsr" : "=a" (low), "=d" (high) : "c" (APL_UCODE_CRBUS_UNLOCK));
	printk(BIOS_INFO, "MSR value: 0x%x%x\n", high, low);
}
