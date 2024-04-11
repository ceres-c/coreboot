/*
 * Federico (ceres-c) Cerutti
*/

#include <bootblock_common.h>
#include <console/console.h>
#include <cpu/x86/msr.h>

#include "lib-micro-x86/misc.h"
#include "lib-micro-x86/ucode_macro.h"
#include "lib-micro-x86/udbg.h"
#include "lib-micro-x86/opcode.h"
#include "lib-micro-x86/match_and_patch_hook.h"

#define MAGIC_UNLOCK 0x200

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



void bootblock_red_unlock_payload(void)
{
	/* NOTE: I don't do any exception handling, that's why IDT_IN_EVERY_STAGE is enabled with RED_UNLOCK
	 * If any weird exception is thrown, check that your system is actually red unlocked (right blob?)
	 */
	unsigned int low = 0, high = 0;
	__asm__ volatile ("rdmsr" : "=a" (low), "=d" (high) : "c" (APL_UCODE_CRBUS_UNLOCK));
	printk(BIOS_INFO, "MSR value: 0x%x%x\n", high, low);
	
	/* Enable ucode debug */
	__asm__ volatile ("wrmsr" : : "a" (MAGIC_UNLOCK), "d" (0), "c" (APL_UCODE_CRBUS_UNLOCK));

	/* Check if it worked */
	__asm__ volatile ("rdmsr" : "=a" (low), "=d" (high) : "c" (APL_UCODE_CRBUS_UNLOCK));
	printk(BIOS_INFO, "MSR value: 0x%x%x\n", high, low);
	if (high != 0 || low != MAGIC_UNLOCK) {
		die("\tFailed to write magic CRBUS MSR\n");
	}

	// crbus_write(PMH_CR_BRAM_BASE, 0x1);
	// printk(BIOS_INFO, "Crbus written\n");
	// do_fix_IN_patch();
	// printk(BIOS_INFO, "IN patched\n");
	// do_rdrand_patch();
	// printk(BIOS_INFO, "RDRAND patched\n");

	// uint32_t rand = 0x11223344;
	// asm volatile("rdrand %0\n":"=r"(rand):);
	// printk(BIOS_INFO, "rdrand eax: 0x%08x\n", rand);
	// if (rand != 0x1337)
	// 	die("Failed to patch microcode\n");
}
