#include "lib-micro-minimal.h"

uint32_t match_and_patch_hook_addr = 0x7de0;
ucode_t match_and_patch_hook_ucode_patch[] = {
    {   // 0x7de0
        MOVE_DSZ64_DR(TMP0, RDI),
        MOVE_DSZ64_DR(TMP1, RSI),
        ZEROEXT_DSZ32_DI(TMP10, 0x0),
        NOP_SEQWORD
    },
    {   // 0x7de4
        // pause frontend
        MOVEFROMCREG_DSZ64_DI(TMP12, 0x38c),
        MOVETOCREG_DSZ64_RI(TMP10, 0x38c),
        // write match&patch at location tmp1 with the value of tmp0
        ZEROEXT_DSZ32_DI(TMP9, 0x303),
        NOP_SEQWORD
    },
    {   // 0x7de8
        SHL_DSZ32_DRI(TMP9, TMP9, 0x8),
        MOVETOCREG_DSZ64_RI(TMP9, 0x6a1),
        MOVETOCREG_DSZ64_RI(TMP1, 0x6a0),
        NOP_SEQWORD
    },
    {   // 0x7dec
        MOVETOCREG_DSZ64_RI(TMP0, 0x6a4),
        MOVETOCREG_DSZ64_RI(TMP10, 0x6a4),
        MOVETOCREG_DSZ64_RI(TMP10, 0x6a1),
        NOP_SEQWORD
    },
    {   // 0x7df0
        // restore frontend
        MOVETOCREG_DSZ64_RI(TMP12, 0x38c),
        ZEROEXT_DSZ32_DI(RAX, 0x1337),
        CONCAT_DSZ16_DRI(RAX, RAX, 0x1337),
        END_SEQWORD
    },
};

/* Make GCC happy with prototypes */
uint32_t ucode_addr_to_patch_addr(uint32_t addr);
uint32_t ucode_addr_to_patch_seqword_addr(uint32_t addr);
void ldat_array_write(uint32_t pdat_reg, uint32_t array_sel, uint32_t bank_sel, uint32_t dword_idx, uint32_t fast_addr, uint64_t val);
void ms_array_write(uint32_t array_sel, uint32_t bank_sel, uint32_t dword_idx, uint32_t fast_addr, uint64_t val);
static void ms_array_4_write(uint32_t addr, uint64_t val);
static void ms_array_2_write(uint32_t addr, uint64_t val);

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
