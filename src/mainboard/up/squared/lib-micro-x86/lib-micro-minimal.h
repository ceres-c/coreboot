#ifndef LIB_MICRO_MINIMAL_H
#define LIB_MICRO_MINIMAL_H

#include <stdint.h>
#include <console/console.h>
#include <cpu/x86/msr.h>

#include "misc.h"
#include "ucode_macro.h"
#include "udbg.h"
#include "opcode.h"
#include "inst.h"

#define MAGIC_UNLOCK 0x200 // See "Undocumented x86 Instructions..." by Ermolov et al.

#define MATCH_AND_PATCH_HOOK_ADDR 0x7de0 // URAM address where the match and patch hook is placed

extern ucode_t match_and_patch_hook_ucode_patch[];

uint32_t ucode_addr_to_patch_addr(uint32_t addr);
uint32_t ucode_addr_to_patch_seqword_addr(uint32_t addr);
void ldat_array_write(uint32_t pdat_reg, uint32_t array_sel, uint32_t bank_sel, uint32_t dword_idx, uint32_t fast_addr, uint64_t val);
void ms_array_write(uint32_t array_sel, uint32_t bank_sel, uint32_t dword_idx, uint32_t fast_addr, uint64_t val);
void ms_array_4_write(uint32_t addr, uint64_t val);
void ms_array_2_write(uint32_t addr, uint64_t val);

void patch_ucode(uint32_t addr, ucode_t ucode_patch[], int n);
void hook_match_and_patch(uint32_t entry_idx, uint32_t ucode_addr, uint32_t patch_addr);
void wrmrs_enable_debug(void);
void do_fix_IN_patch(void);
void apply_patch(uint32_t ucode_msrom_addr, uint32_t ucode_msram_addr, ucode_t *ucode_patch, int triad_count);

#endif
