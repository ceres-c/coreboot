#ifndef UCODE_MACRO_H_
#define UCODE_MACRO_H_

#include "xlat.h"
#include "seqword.h"

#define MOD0 (1ULL << 23)
#define MOD1 (1ULL << 44)
#define MOD2 (1ULL << 45)

__attribute__((always_inline))
inline static uint64_t parity0(uint64_t value) {
    uint64_t parity = 0;
    while (value) {
        parity ^= (value & 1);
        value = value >> 2;
    }
    return parity;
}

__attribute__((always_inline))
inline static uint64_t parity1(uint64_t value) {
    uint64_t parity = 0;
    value = value >> 1;
    while (value) {
        parity ^= (value & 1);
        value = value >> 2;
    }
    return parity;
}

#define CRC_UOP_MASK 0x3FFFFFFFFFFFULL
#define CRC_SEQ_MASK 0xFFFFFFFULL

#define IMM_ENCODE_SRC0(src0_id) \
    (((src0_id) & 0xffULL) << 24) | (((src0_id) & 0x1f00)<< 10) | (((src0_id) & 0xe000) >> 13) | (1 << 3)

#define IMM_ENCODE_SRC1(src1_id) \
    (((src1_id) & 0xffULL) << 24) | (((src1_id) & 0x1f00)<< 10) | (((src1_id) & 0xe000) >> 7) | (1 << 9)

#define IMM0_ENCODE(imm) \
    (((imm)&0xffULL) << 24)

#define IMM1_ENCODE(imm) \
    (((imm)&0x1fULL) << 18) 

#define SRC0_ENCODE(val) \
    (((val) & 0x3f) << 0)

#define SRC1_ENCODE(val) \
    (((val) & 0x3f) << 6)

#define DST_ENCODE(val) \
    (((val) & 0x3f) << 12)

#define CRC_SEQ(seq) \
    (( (seq) & CRC_SEQ_MASK) | ((parity0((seq)&CRC_SEQ_MASK) << 28) | (parity1((seq)&CRC_SEQ_MASK) << 29)))

#define CRC_UOP(uop) \
    (( (uop) & CRC_UOP_MASK) | (parity0((uop)&CRC_UOP_MASK) << 46) | (parity1((uop)&CRC_UOP_MASK) << 47))


#define INSTR_I0(imm) ( IMM_ENCODE_SRC0(imm) )
#define INSTR_I1(imm) ( IMM_ENCODE_SRC1(imm) )
#define INSTR_R0(src) ( SRC0_ENCODE(src) )
#define INSTR_R1(src) ( SRC1_ENCODE(src) )
#define INSTR_M0(macro) ( IMM_ENCODE_SRC0(macro) | MOD0 )
#define INSTR_M1(macro) ( IMM_ENCODE_SRC1(macro) | MOD0 )
#define INSTR_DI0(dst, imm) ( DST_ENCODE(dst) | IMM_ENCODE_SRC0(imm) )
#define INSTR_DI1(dst, imm) ( DST_ENCODE(dst) | IMM_ENCODE_SRC1(imm) )
#define INSTR_DR0(dst, src) ( DST_ENCODE(dst) | SRC0_ENCODE(src) )
#define INSTR_DR1(dst, src) ( DST_ENCODE(dst) | SRC1_ENCODE(src) )
#define INSTR_DM0(dst, macro) ( DST_ENCODE(dst) | IMM_ENCODE_SRC0(macro) | MOD0 )
#define INSTR_DM1(dst, macro) ( DST_ENCODE(dst) | IMM_ENCODE_SRC1(macro) | MOD0 )
#define INSTR_RR(src0, src1) ( SRC0_ENCODE(src0) | SRC1_ENCODE(src1) )
#define INSTR_RI(src, imm) ( SRC0_ENCODE(src) | IMM_ENCODE_SRC1(imm) )
#define INSTR_IR(imm, src) ( IMM_ENCODE_SRC0(imm) | SRC1_ENCODE(src) )
#define INSTR_RM(src, macro) ( SRC0_ENCODE(src) | IMM_ENCODE_SRC1(macro) | MOD0 )
#define INSTR_MR(macro, src) ( IMM_ENCODE_SRC0(macro) | SRC1_ENCODE(src) | MOD0 )
#define INSTR_DRR(dst, src0, src1) ( DST_ENCODE(dst) | SRC0_ENCODE(src0) | SRC1_ENCODE(src1) )
#define INSTR_DRI(dst, src, imm) ( DST_ENCODE(dst) | SRC0_ENCODE(src) | IMM_ENCODE_SRC1(imm) )
#define INSTR_DIR(dst, imm, src) ( DST_ENCODE(dst) | IMM_ENCODE_SRC0(imm) | SRC1_ENCODE(src) )
#define INSTR_DRM(dst, src, macro) ( DST_ENCODE(dst) | SRC0_ENCODE(src) | IMM_ENCODE_SRC1(macro) | MOD0 )
#define INSTR_DMR(dst, macro, src) ( DST_ENCODE(dst) | IMM_ENCODE_SRC0(macro) | SRC1_ENCODE(src) | MOD0 )

#define MEMOP_ENCODE(dst, base_reg, index_reg, offset, seg) \
    ( DST_ENCODE(dst) | SRC0_ENCODE(base_reg) | SRC1_ENCODE(index_reg) | IMM0_ENCODE(offset) | IMM1_ENCODE(seg) )

#endif // UCODE_MACRO_H_
