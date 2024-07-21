/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __CPU__INTEL__MICROCODE__
#define __CPU__INTEL__MICROCODE__

#include <stdint.h>
#include <cpu/x86/msr.h>

struct microcode {
	u32 hdrver;	/* Header Version */
	u32 rev;	/* Update Revision */
	u32 date;	/* Date */
	u32 sig;	/* Processor Signature */

	u32 cksum;	/* Checksum */
	u32 ldrver;	/* Loader Revision */
	u32 pf;		/* Processor Flags */

	u32 data_size;	/* Data Size */
	u32 total_size;	/* Total Size */

	u32 reserved[3];
};

static inline u32 read_microcode_rev(void)
{
	/* Some Intel CPUs can be very finicky about the
	 * CPUID sequence used.  So this is implemented in
	 * assembly so that it works reliably.
	 */
	msr_t msr;
	asm volatile (
		"xorl %%eax, %%eax\n\t"
		"xorl %%edx, %%edx\n\t"
		"movl $0x8b, %%ecx\n\t"
		"wrmsr\n\t"
		"movl $0x01, %%eax\n\t"
		"cpuid\n\t"
		"movl $0x08b, %%ecx\n\t"
		"rdmsr\n\t"
		: /* outputs */
		"=a" (msr.lo), "=d" (msr.hi)
		: /* inputs */
		: /* trashed */
		 "ebx", "ecx"
	);
	return msr.hi;
}

/* Find the microcode and reload the microcode if SoC has RELOAD_MICROCODE_PATCH
 * config selected. */
void intel_reload_microcode(void);

void intel_update_microcode_from_cbfs(void);
/* Find a microcode that matches the revision and platform family returning
 * NULL if none found. The found microcode is cached for faster access on
 * subsequent calls of this function.
 *
 * Since this function caches the found microcode (NULL or a valid microcode
 * pointer), it is expected to be run from BSP before starting any other APs.
 * It is not multithread safe otherwise. */
const void *intel_microcode_find(void);

/* It is up to the caller to determine if parallel loading is possible as
 * well as ensuring the microcode matches the family and revision (i.e. with
 * intel_microcode_find()). */
void intel_microcode_load_unlocked(const void *microcode_patch);

/* SoC specific check to determine if microcode update is really
 * required, will skip microcode update if true. */
int soc_skip_ucode_update(u32 current_patch_id, u32 new_patch_id);

/* return the version of the currently running microcode */
uint32_t get_current_microcode_rev(void);

/* extract microcode revision from the given patch */
uint32_t get_microcode_rev(const void *microcode);
/* extract microcode size from the given patch */
uint32_t get_microcode_size(const void *microcode);
/* extract checksum from the given patch */
uint32_t get_microcode_checksum(const void *microcode);
#endif
