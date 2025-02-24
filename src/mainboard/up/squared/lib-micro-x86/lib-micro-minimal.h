#ifndef LIB_MICRO_MINIMAL_H
#define LIB_MICRO_MINIMAL_H

#include <stdint.h>
#include <console/console.h>

#include "misc.h"
#include "ucode_macro.h"
#include "udbg.h"
#include "opcode.h"
#include "inst.h"

void patch_ucode(uint32_t addr, ucode_t ucode_patch[], int n);
void hook_match_and_patch(uint32_t entry_idx, uint32_t ucode_addr, uint32_t patch_addr);
void do_fix_IN_patch(void);

#endif
