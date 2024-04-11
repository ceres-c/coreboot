#ifndef OPCODE_H_
#define OPCODE_H_

// registers
#define RAX     0x20ULL
#define RCX     0x21ULL
#define RDX     0x22ULL
#define RBX     0x23ULL
#define RSP     0x24ULL
#define RBP     0x25ULL
#define RSI     0x26ULL
#define RDI     0x27ULL
#define R8      0x28ULL
#define R9      0x29ULL
#define R10     0x2aULL
#define R11     0x2bULL
#define R12     0x2cULL
#define R13     0x2dULL
#define R14     0x2eULL
#define R15     0x2fULL
#define TMP0    0x30ULL
#define TMP1    0x31ULL
#define TMP2    0x32ULL
#define TMP3    0x33ULL
#define TMP4    0x34ULL
#define TMP5    0x35ULL
#define TMP6    0x36ULL
#define TMP7    0x37ULL
#define TMP8    0x38ULL
#define TMP9    0x39ULL
#define TMP10   0x3aULL
#define TMP11   0x3bULL
#define TMP12   0x3cULL
#define TMP13   0x3dULL
#define TMP14   0x3eULL
#define TMP15   0x3fULL
#define XMM0    0x20ULL
#define XMM1    0x21ULL
#define XMM2    0x22ULL
#define XMM3    0x23ULL
#define XMM4    0x24ULL
#define XMM5    0x25ULL
#define XMM6    0x26ULL
#define XMM7    0x27ULL
#define XMM8    0x28ULL
#define XMM9    0x29ULL
#define XMM10   0x2aULL
#define XMM11   0x2bULL
#define XMM12   0x2cULL
#define XMM13   0x2dULL
#define XMM14   0x2eULL
#define XMM15   0x2fULL
#define MM0     0x30ULL
#define MM1     0x31ULL
#define MM2     0x32ULL
#define MM3     0x33ULL
#define MM4     0x34ULL
#define MM5     0x35ULL
#define MM6     0x36ULL
#define MM7     0x37ULL
#define TMM0    0x38ULL
#define TMM1    0x39ULL
#define TMM2    0x3aULL
#define TMM3    0x3bULL
#define TMM4    0x3cULL
#define TMM5    0x3dULL
#define TMM6    0x3eULL
#define TMM7    0x3fULL
#define R64DST  0x01ULL
#define R64SRC  0x02ULL
#define R64BASE 0x04ULL
#define R64IDX  0x05ULL
#define TMPV0   0x14ULL
#define TMPV1   0x15ULL
#define TMPV2   0x16ULL
#define TMPV3   0x17ULL

// control registers
#define CORE_CR_CUR_RIP 0x67ULL
#define CORE_CR_CUR_UIP 0x68ULL
#define UCODE_CR_SE_SVN_KEY_0 0x205ULL
#define UCODE_CR_SE_SVN_KEY_1 0x206ULL
#define CTAP_CR_PPPE_TAP_STATUS 0x208ULL
#define CTAP_CR_TAP_CONFIG 0x285ULL
#define UCODE_CR_X2APIC_TPR 0x288ULL
#define UCODE_CR_X2APIC_EOI 0x28bULL
#define UCODE_CR_PPPE_EVENT_RESET 0x29eULL
#define UCODE_CR_PPPE_EVENT_STATUS 0x29fULL
#define CTAP_CR_PDR_T0_LOW 0x2c0ULL
#define CTAP_CR_PDR_T0_HIGH 0x2c1ULL
#define ML3_CR_PIC_GLOBAL_EVENT_INHIBIT 0x2c4ULL
#define CTAP_CR_PROBE_MODE 0x2cdULL
#define X86_CR_THREAD_ID 0x2b9ULL
#define ROB1_CR_ICECTLPMR 0x2dfULL
#define ML3_CR_PIC_DEBUG_MODES 0x2e6ULL
#define PMH_CR_BRAM_BASE 0x51bULL
#define PMH_CR_CR3 0x529ULL
#define PMH_CR_ELSRR_BASE 0x555ULL
#define PMH_CR_ELSRR_MASK 0x556ULL
#define PMH_CR_EPCM_BASE 0x557ULL
#define PMH_CR_EMRR_BASE 0x562ULL
#define PMH_CR_EMRR_MASK 0x563ULL
#define PMH_CR_EMXRR_BASE 0x564ULL
#define PMH_CR_EMXRR_MASK 0x565ULL
#define PMH_CR_SMRR_BASE 0x572ULL
#define PMH_CR_SMRR_MASK 0x573ULL
#define PMH_CR_PRMRR_BASE 0x574ULL
#define PMH_CR_PRMRR_MASK 0x575ULL
#define MS_CR_DEBUG_DEFEATURE 0x692ULL
#define XU_CR_MACROINSTRUCTION_ALIAS 0x752ULL
#define CORE_CR_CR4 0x7c5ULL
#define BAC_CR_CS_BASE 0x78eULL
#define UCODE_CR_XU_USTATE_CTRL 0x7c6ULL
#define CORE_CR_CR0 0x7f6ULL
#define CORE_CR_DEBUGCTL 0x7f9ULL
#define CORE_CR_EFLAGS 0x7feULL

// macro imms
#define IMM_MACRO_ALIAS_DISPLACEMENT    0x00ULL
#define IMM_MACRO_ALIAS_IMMEDIATE       0x00ULL
#define IMM_MACRO_ALIAS_RIP             0x04ULL
#define IMM_MACRO_ALIAS_STI             0x05ULL
#define IMM_MACRO_ALIAS_MSLOOPCTR       0x08ULL
#define IMM_MACRO_ALIAS_DATASIZE        0x0bULL
#define IMM_MACRO_ALIAS_INSTRUCTION     0x10ULL

// ustate UCODE
#define UST_MSLOOPCTR_NONZERO   0x0001ULL
#define UST_SMM                 0x0800ULL

// ustate SYS
#define UST_VMX_DIS         0x0001ULL
#define UST_USER_MODE       0x0002ULL
#define UST_8086_MODE       0x0004ULL
#define UST_OP_SIZE_32BIT   0x0008ULL
#define UST_ADDR_SIZE_64BIT 0x0010ULL
#define UST_XUCODE          0x0020ULL
#define UST_SE_INIT_DONE    0x0040ULL
#define UST_VMX_GUEST       0x0080ULL
#define UST_VMX_OP_DIS      0x0100ULL

// Segment selectors
#define SEG_PHYS 0x01ULL
#define SEG_GDT  0x06ULL
#define SEG_LDT  0x07ULL
#define SEG_ES   0x08ULL
#define SEG_CS   0x09ULL
#define SEG_FS   0x0cULL
#define SEG_GS   0x0dULL
#define SEG_IDT  0x0eULL
#define SEG_TSS  0x0fULL
#define SEG_DS   0x18ULL
#define SEG_SS   0x1aULL

// Segment descriptor fields
#define FLD_LIMIT_VAL    0x0ULL
#define FLD_BASE         0x2ULL
#define FLD_FLGS         0x4ULL
#define FLD_LIMIT        0x6ULL
#define FLD_SEL          0x8ULL
#define FLD_SEL_FLGS_LIM 0xaULL

// opcode parts
#define _CONDO      0x000ULL
#define _CONDNO     0x100ULL
#define _CONDB      0x200ULL
#define _CONDNB     0x300ULL
#define _CONDZ      0x001ULL
#define _CONDNZ     0x101ULL
#define _CONDBE     0x201ULL
#define _CONDNBE    0x301ULL
#define _CONDS      0x002ULL
#define _CONDNS     0x102ULL
#define _CONDP      0x202ULL
#define _CONDNP     0x302ULL
#define _CONDL      0x003ULL
#define _CONDNL     0x103ULL
#define _CONDLE     0x203ULL
#define _CONDNLE    0x303ULL

#define _DSZ32  0x000ULL
#define _DSZ64  0x040ULL
#define _DSZ16  0x080ULL
#define _DSZ8   0x0c0ULL

// opcodes
#define NOP (0x0)

#define _ADD_DSZ32 (0x000ULL << 32)
#define _OR_DSZ32 (0x001ULL << 32)
#define _AND_DSZ32 (0x004ULL << 32)
#define _SUB_DSZ32 (0x005ULL << 32)
#define _XOR_DSZ32 (0x006ULL << 32)
#define _NOTAND_DSZ32 (0x007ULL << 32)
#define _ZEROEXT_DSZ32 (0x008ULL << 32)
#define _MOVE_DSZ32 (0x009ULL << 32)
/* #define _TESTUSTATE (0x04aULL << 32) */
#define _TESTUSTATE (0x00aULL << 32)
#define _UPDATEUSTATE (0x00bULL << 32)
#define _SAVEUIP (0x00cULL << 32)
#define _SAVEUIP_REGOVR (0x00dULL << 32)
#define _WRMSLOOPCTRFBR (0x00eULL << 32)
#define _BT_DSZ32 (0x014ULL << 32)
#define _BTS_DSZ32 (0x015ULL << 32)
#define _BTR_DSZ32 (0x016ULL << 32)
#define _BTC_DSZ32 (0x017ULL << 32)
#define _MJMPTARGET_INDIRECT_ASZ32 (0x01eULL << 32)
#define _CONCAT_DSZ32 (0x021ULL << 32)
#define _SHL_DSZ32 (0x024ULL << 32)
#define _SHR_DSZ32 (0x025ULL << 32)
#define _MOVSX_SSZ32_DSZ64 (0x02aULL << 32)
#define _ROL_DSZ32 (0x02cULL << 32)
#define _ROR_DSZ32 (0x02dULL << 32)
#define _SAR_DSZ32 (0x02eULL << 32)
#define _SELECTCC_DSZ32_CONDO (0x030ULL << 32)
#define _SELECTCC_DSZ32_CONDNO (0x031ULL << 32)
#define _SELECTCC_DSZ32_CONDB (0x032ULL << 32)
#define _SELECTCC_DSZ32_CONDNB (0x033ULL << 32)
#define _CMOVCC_DSZ32_CONDO (0x034ULL << 32)
#define _CMOVCC_DSZ32_CONDNO (0x035ULL << 32)
#define _CMOVCC_DSZ32_CONDB (0x036ULL << 32)
#define _CMOVCC_DSZ32_CONDNB (0x037ULL << 32)
#define _MOVEINSERTFLGS_DSZ32 (0x03dULL << 32)
#define _ADD_DSZ64 (0x040ULL << 32)
#define _OR_DSZ64 (0x041ULL << 32)
#define _MOVETOCREG_DSZ64 (0x042ULL << 32)
#define _WRITEURAM (0x043ULL << 32)
#define _AND_DSZ64 (0x044ULL << 32)
#define _SUB_DSZ64 (0x045ULL << 32)
#define _XOR_DSZ64 (0x046ULL << 32)
#define _NOTAND_DSZ64 (0x047ULL << 32)
#define _ZEROEXT_DSZ64 (0x048ULL << 32)
#define _MOVE_DSZ64 (0x049ULL << 32)
/* #define _TESTUSTATE (0x04aULL << 32) */
/* #define _SAVEUIP (0x04cULL << 32) */
#define _UJMPCC_DIRECT_NOTTAKEN_CONDO (0x050ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDNO (0x051ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDB (0x052ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDNB (0x053ULL << 32)
#define _BT_DSZ64 (0x054ULL << 32)
#define _BTS_DSZ64 (0x055ULL << 32)
#define _BTR_DSZ64 (0x056ULL << 32)
#define _BTC_DSZ64 (0x057ULL << 32)
#define _MJMPCC_DSZNOP_CONDO (0x058ULL << 32)
#define _MJMPCC_DSZNOP_CONDNO (0x059ULL << 32)
#define _MJMPCC_DSZNOP_CONDB (0x05aULL << 32)
#define _MJMPCC_DSZNOP_CONDNB (0x05bULL << 32)
#define _MJMPTARGET_INDIRECT_ASZ64 (0x05cULL << 32)
/* #define _MJMPTARGET_INDIRECT_ASZ64 (0x05eULL << 32) */
#define _MOVEFROMCREG_DSZ64 (0x062ULL << 32)
#define _READURAM (0x063ULL << 32)
#define _SHL_DSZ64 (0x064ULL << 32)
#define _SHR_DSZ64 (0x065ULL << 32)
#define _ROL_DSZ64 (0x06cULL << 32)
#define _ROR_DSZ64 (0x06dULL << 32)
#define _SAR_DSZ64 (0x06eULL << 32)
#define _SELECTCC_DSZ64_CONDO (0x070ULL << 32)
#define _SELECTCC_DSZ64_CONDNO (0x071ULL << 32)
#define _SELECTCC_DSZ64_CONDB (0x072ULL << 32)
#define _SELECTCC_DSZ64_CONDNB (0x073ULL << 32)
#define _CMOVCC_DSZ64_CONDO (0x074ULL << 32)
#define _CMOVCC_DSZ64_CONDNO (0x075ULL << 32)
#define _CMOVCC_DSZ64_CONDB (0x076ULL << 32)
#define _CMOVCC_DSZ64_CONDNB (0x077ULL << 32)
#define _MOVEINSERTFLGS_DSZ64 (0x07dULL << 32)
#define _ADD_DSZ16 (0x080ULL << 32)
#define _OR_DSZ16 (0x081ULL << 32)
#define _AND_DSZ16 (0x084ULL << 32)
#define _SUB_DSZ16 (0x085ULL << 32)
#define _XOR_DSZ16 (0x086ULL << 32)
#define _NOTAND_DSZ16 (0x087ULL << 32)
#define _ZEROEXT_DSZ16 (0x088ULL << 32)
/* #define _SAVEUIP (0x08cULL << 32) */
#define _BT_DSZ16 (0x094ULL << 32)
#define _BTS_DSZ16 (0x095ULL << 32)
#define _BTR_DSZ16 (0x096ULL << 32)
#define _BTC_DSZ16 (0x097ULL << 32)
#define _CONCAT_DSZ16 (0x0a1ULL << 32)
#define _SHL_DSZ16 (0x0a4ULL << 32)
#define _SHR_DSZ16 (0x0a5ULL << 32)
#define _MOVSX_DSZ16 (0x0a8ULL << 32)
#define _MOVZX_DSZ16 (0x0a9ULL << 32)
#define _ROL_DSZ16 (0x0acULL << 32)
#define _ROR_DSZ16 (0x0adULL << 32)
#define _SAR_DSZ16 (0x0aeULL << 32)
#define _CMOVCC_DSZ16_CONDO (0x0b4ULL << 32)
#define _CMOVCC_DSZ16_CONDNO (0x0b5ULL << 32)
#define _CMOVCC_DSZ16_CONDB (0x0b6ULL << 32)
#define _CMOVCC_DSZ16_CONDNB (0x0b7ULL << 32)
#define _ADD_DSZ8 (0x0c0ULL << 32)
#define _OR_DSZ8 (0x0c1ULL << 32)
#define _AND_DSZ8 (0x0c4ULL << 32)
#define _SUB_DSZ8 (0x0c5ULL << 32)
#define _XOR_DSZ8 (0x0c6ULL << 32)
#define _NOTAND_DSZ8 (0x0c7ULL << 32)
#define _ZEROEXT_DSZ8 (0x0c8ULL << 32)
/* #define _SAVEUIP (0x0ccULL << 32) */
#define _NEG_DSZ8 (0x0e0ULL << 32)
#define _CONCAT_DSZ8 (0x0e1ULL << 32)
#define _SHL_DSZ8 (0x0e4ULL << 32)
#define _SHR_DSZ8 (0x0e5ULL << 32)
#define _MOVSX_DSZ8 (0x0e8ULL << 32)
#define _MOVZX_DSZ8 (0x0e9ULL << 32)
#define _ROL_DSZ8 (0x0ecULL << 32)
#define _ROR_DSZ8 (0x0edULL << 32)
#define _SAR_DSZ8 (0x0eeULL << 32)
#define _CMOVCC_DSZ8_CONDO (0x0f4ULL << 32)
#define _CMOVCC_DSZ8_CONDNO (0x0f5ULL << 32)
#define _CMOVCC_DSZ8_CONDB (0x0f6ULL << 32)
#define _CMOVCC_DSZ8_CONDNB (0x0f7ULL << 32)
#define _SETCC_CONDO (0x0f8ULL << 32)
#define _SETCC_CONDNO (0x0f9ULL << 32)
#define _SETCC_CONDB (0x0faULL << 32)
#define _SETCC_CONDNB (0x0fbULL << 32)
#define _READUIP_REGOVR (0x108ULL << 32)
#define _SUBR_DSZ32 (0x120ULL << 32)
#define _RCXBTCNTMSK_DSZ32 (0x122ULL << 32)
#define _SELECTCC_DSZ32_CONDZ (0x130ULL << 32)
#define _SELECTCC_DSZ32_CONDNZ (0x131ULL << 32)
#define _SELECTCC_DSZ32_CONDBE (0x132ULL << 32)
#define _SELECTCC_DSZ32_CONDNBE (0x133ULL << 32)
#define _CMOVCC_DSZ32_CONDZ (0x134ULL << 32)
#define _CMOVCC_DSZ32_CONDNZ (0x135ULL << 32)
#define _CMOVCC_DSZ32_CONDBE (0x136ULL << 32)
#define _CMOVCC_DSZ32_CONDNBE (0x137ULL << 32)
#define _MOVEMERGEFLGS_DSZ32 (0x13eULL << 32)
#define _UFLOWCTRL (0x142ULL << 32)
#define _AETTRACE (0x143ULL << 32)
#define _URET (0x148ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDZ (0x150ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDNZ (0x151ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDBE (0x152ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDNBE (0x153ULL << 32)
#define _MJMPCC_DSZNOP_CONDZ (0x158ULL << 32)
#define _MJMPCC_DSZNOP_CONDNZ (0x159ULL << 32)
#define _MJMPCC_DSZNOP_CONDBE (0x15aULL << 32)
#define _MJMPCC_DSZNOP_CONDNBE (0x15bULL << 32)
#define _UJMP (0x15dULL << 32)
#define _UJMPCC_DIRECT_TAKEN_CONDZ (0x15fULL << 32)
#define _SUBR_DSZ64 (0x160ULL << 32)
#define _RCXBTCNTMSK_DSZ64 (0x162ULL << 32)
#define _SELECTCC_DSZ64_CONDZ (0x170ULL << 32)
#define _SELECTCC_DSZ64_CONDNZ (0x171ULL << 32)
#define _SELECTCC_DSZ64_CONDBE (0x172ULL << 32)
#define _SELECTCC_DSZ64_CONDNBE (0x173ULL << 32)
#define _CMOVCC_DSZ64_CONDZ (0x174ULL << 32)
#define _CMOVCC_DSZ64_CONDNZ (0x175ULL << 32)
#define _CMOVCC_DSZ64_CONDBE (0x176ULL << 32)
#define _CMOVCC_DSZ64_CONDNBE (0x177ULL << 32)
#define _MOVEMERGEFLGS_DSZ64 (0x17eULL << 32)
#define _ADDSUB_DSZ16_CONDD (0x189ULL << 32)
#define _SUBR_DSZ16 (0x1a0ULL << 32)
#define _RCXBTCNTMSK_DSZ16 (0x1a2ULL << 32)
#define _CMOVCC_DSZ16_CONDZ (0x1b4ULL << 32)
#define _CMOVCC_DSZ16_CONDNZ (0x1b5ULL << 32)
#define _CMOVCC_DSZ16_CONDBE (0x1b6ULL << 32)
#define _CMOVCC_DSZ16_CONDNBE (0x1b7ULL << 32)
#define _SAHF (0x1bfULL << 32)
#define _SUBR_DSZ8 (0x1e0ULL << 32)
#define _RCXBTCNTMSK_DSZ8 (0x1e2ULL << 32)
#define _CMOVCC_DSZ8_CONDZ (0x1f4ULL << 32)
#define _CMOVCC_DSZ8_CONDNZ (0x1f5ULL << 32)
#define _CMOVCC_DSZ8_CONDBE (0x1f6ULL << 32)
#define _CMOVCC_DSZ8_CONDNBE (0x1f7ULL << 32)
#define _SETCC_CONDZ (0x1f8ULL << 32)
#define _SETCC_CONDNZ (0x1f9ULL << 32)
#define _SETCC_CONDBE (0x1faULL << 32)
#define _SETCC_CONDNBE (0x1fbULL << 32)
#define _PSELECT_CPL0 (0x202ULL << 32)
#define _FETCHFROMEIP0_ASZ32 (0x214ULL << 32)
#define _FETCHFROMEIP1_ASZ32 (0x215ULL << 32)
#define _SIGEVENT (0x21eULL << 32)
#define _IMUL32L_DSZ32 (0x225ULL << 32)
#define _MSR2CR (0x228ULL << 32)
#define _MUL_DSZ32 (0x22cULL << 32)
#define _IMUL_DSZ32 (0x22dULL << 32)
#define _SELECTCC_DSZ32_CONDS (0x230ULL << 32)
#define _SELECTCC_DSZ32_CONDNS (0x231ULL << 32)
#define _SELECTCC_DSZ32_CONDP (0x232ULL << 32)
#define _SELECTCC_DSZ32_CONDNP (0x233ULL << 32)
#define _CMOVCC_DSZ32_CONDS (0x234ULL << 32)
#define _CMOVCC_DSZ32_CONDNS (0x235ULL << 32)
#define _CMOVCC_DSZ32_CONDP (0x236ULL << 32)
#define _CMOVCC_DSZ32_CONDNP (0x237ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDS (0x250ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDNS (0x251ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDP (0x252ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDNP (0x253ULL << 32)
#define _FETCHFROMEIP0_ASZ64 (0x254ULL << 32)
#define _FETCHFROMEIP1_ASZ64 (0x255ULL << 32)
#define _MJMPCC_DSZNOP_CONDS (0x258ULL << 32)
#define _MJMPCC_DSZNOP_CONDNS (0x259ULL << 32)
#define _MJMPCC_DSZNOP_CONDP (0x25aULL << 32)
#define _MJMPCC_DSZNOP_CONDNP (0x25bULL << 32)
#define _TEST_DSZ64 (0x25dULL << 32)
#define _IMUL64L_DSZ64 (0x264ULL << 32)
#define _RDVMCSPLA (0x269ULL << 32)
#define _SELECTCC_DSZ64_CONDS (0x270ULL << 32)
#define _SELECTCC_DSZ64_CONDNS (0x271ULL << 32)
#define _SELECTCC_DSZ64_CONDP (0x272ULL << 32)
#define _SELECTCC_DSZ64_CONDNP (0x273ULL << 32)
#define _CMOVCC_DSZ64_CONDS (0x274ULL << 32)
#define _CMOVCC_DSZ64_CONDNS (0x275ULL << 32)
#define _CMOVCC_DSZ64_CONDP (0x276ULL << 32)
#define _CMOVCC_DSZ64_CONDNP (0x277ULL << 32)
#define _CMOVCC_DSZ16_CONDS (0x2b4ULL << 32)
#define _CMOVCC_DSZ16_CONDNS (0x2b5ULL << 32)
#define _CMOVCC_DSZ16_CONDP (0x2b6ULL << 32)
#define _CMOVCC_DSZ16_CONDNP (0x2b7ULL << 32)
#define _CMOVCC_DSZ8_CONDS (0x2f4ULL << 32)
#define _CMOVCC_DSZ8_CONDNS (0x2f5ULL << 32)
#define _CMOVCC_DSZ8_CONDP (0x2f6ULL << 32)
#define _CMOVCC_DSZ8_CONDNP (0x2f7ULL << 32)
#define _SETCC_CONDS (0x2f8ULL << 32)
#define _SETCC_CONDNS (0x2f9ULL << 32)
#define _SETCC_CONDP (0x2faULL << 32)
#define _SETCC_CONDNP (0x2fbULL << 32)
#define _RCL_DSZ32 (0x32eULL << 32)
#define _SELECTCC_DSZ32_CONDL (0x330ULL << 32)
#define _SELECTCC_DSZ32_CONDNL (0x331ULL << 32)
#define _SELECTCC_DSZ32_CONDLE (0x332ULL << 32)
#define _SELECTCC_DSZ32_CONDNLE (0x333ULL << 32)
#define _CMOVCC_DSZ32_CONDL (0x334ULL << 32)
#define _CMOVCC_DSZ32_CONDNL (0x335ULL << 32)
#define _CMOVCC_DSZ32_CONDLE (0x336ULL << 32)
#define _CMOVCC_DSZ32_CONDNLE (0x337ULL << 32)
#define _CLC (0x338ULL << 32)
#define _CMC (0x339ULL << 32)
#define _STC (0x33aULL << 32)
#define _BSWAP_DSZ32 (0x33cULL << 32)
#define _ADC (0x33eULL << 32)
#define _SBB (0x33fULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDL (0x350ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDNL (0x351ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDLE (0x352ULL << 32)
#define _UJMPCC_DIRECT_NOTTAKEN_CONDNLE (0x353ULL << 32)
#define _MJMPCC_DSZNOP_CONDL (0x358ULL << 32)
#define _MJMPCC_DSZNOP_CONDNL (0x359ULL << 32)
#define _MJMPCC_DSZNOP_CONDLE (0x35aULL << 32)
#define _MJMPCC_DSZNOP_CONDNLE (0x35bULL << 32)
#define _SELECTCC_DSZ64_CONDL (0x370ULL << 32)
#define _SELECTCC_DSZ64_CONDNL (0x371ULL << 32)
#define _SELECTCC_DSZ64_CONDLE (0x372ULL << 32)
#define _SELECTCC_DSZ64_CONDNLE (0x373ULL << 32)
#define _CMOVCC_DSZ64_CONDL (0x374ULL << 32)
#define _CMOVCC_DSZ64_CONDNL (0x375ULL << 32)
#define _CMOVCC_DSZ64_CONDLE (0x376ULL << 32)
#define _CMOVCC_DSZ64_CONDNLE (0x377ULL << 32)
#define _GENARITHFLAGS (0x37dULL << 32)
#define _READAFLAGS (0x380ULL << 32)
#define _RCL_DSZ16 (0x3aeULL << 32)
#define _CMOVCC_DSZ16_CONDL (0x3b4ULL << 32)
#define _CMOVCC_DSZ16_CONDNL (0x3b5ULL << 32)
#define _CMOVCC_DSZ16_CONDLE (0x3b6ULL << 32)
#define _CMOVCC_DSZ16_CONDNLE (0x3b7ULL << 32)
#define _LAHF (0x3c0ULL << 32)
#define _INC_DSZ8 (0x3c8ULL << 32)
#define _DEC_DSZ8 (0x3caULL << 32)
#define _RCL_DSZ8 (0x3eeULL << 32)
#define _CMOVCC_DSZ8_CONDL (0x3f4ULL << 32)
#define _CMOVCC_DSZ8_CONDNL (0x3f5ULL << 32)
#define _CMOVCC_DSZ8_CONDLE (0x3f6ULL << 32)
#define _CMOVCC_DSZ8_CONDNLE (0x3f7ULL << 32)
#define _SETCC_CONDL (0x3f8ULL << 32)
#define _SETCC_CONDNL (0x3f9ULL << 32)
#define _SETCC_CONDLE (0x3faULL << 32)
#define _SETCC_CONDNLE (0x3fbULL << 32)
/* #define _ADC (0x3feULL << 32) */
/* #define _SBB (0x3ffULL << 32) */
#define _PSUBSB (0x440ULL << 32)
#define _PSUBSW (0x441ULL << 32)
#define _PMINSW (0x442ULL << 32)
#define _POR (0x443ULL << 32)
#define _PADDSB (0x444ULL << 32)
#define _PADDSW (0x445ULL << 32)
#define _PMAXSW (0x446ULL << 32)
#define _PXOR (0x447ULL << 32)
#define _PSUBB (0x448ULL << 32)
#define _PSUBW (0x449ULL << 32)
#define _PSUBD (0x44aULL << 32)
#define _PADDB (0x44cULL << 32)
#define _PADDW (0x44dULL << 32)
#define _PADDD (0x44eULL << 32)
#define _PCMPGTB (0x450ULL << 32)
#define _PCMPGTW (0x451ULL << 32)
#define _PCMPGTD (0x452ULL << 32)
#define _PALIGNR (0x457ULL << 32)
#define _PCMPEQW (0x458ULL << 32)
#define _PCMPEQB (0x459ULL << 32)
#define _PCMPEQD (0x45aULL << 32)
#define _PADDQ (0x45bULL << 32)
#define _PSUBQ (0x45fULL << 32)
#define _PSUBUSB (0x460ULL << 32)
#define _PSUBUSW (0x461ULL << 32)
#define _PMINUB (0x462ULL << 32)
#define _PAND (0x463ULL << 32)
#define _PADDUSB (0x464ULL << 32)
#define _PADDUSW (0x465ULL << 32)
#define _PMAXUB (0x466ULL << 32)
#define _PANDN (0x467ULL << 32)
#define _PAVGB (0x468ULL << 32)
#define _PAVGW (0x46bULL << 32)
#define _MOVLPD (0x470ULL << 32)
#define _MOVHPD (0x471ULL << 32)
#define _MOVDQU (0x472ULL << 32)
#define _FMOV (0x4b4ULL << 32)
#define _ORPD (0x4c3ULL << 32)
#define _XORPD (0x4c7ULL << 32)
#define _ANDPD (0x4e3ULL << 32)
#define _ANDNPD (0x4e7ULL << 32)
#define _MOVHLPS (0x4efULL << 32)
#define _PUNPCKLDQ (0x508ULL << 32)
#define _PSRLQ (0x50aULL << 32)
#define _PSLLQ (0x50eULL << 32)
#define _CVTPD2PI (0x514ULL << 32)
#define _PUNPCKHDQ (0x518ULL << 32)
/* #define _PSRLQ (0x51aULL << 32) */
#define _PACKSSDW (0x51dULL << 32)
/* #define _PSLLQ (0x51eULL << 32) */
#define _UNPCKHPD (0x528ULL << 32)
#define _UNPCKLPD (0x529ULL << 32)
#define _PMADDWD (0x556ULL << 32)
#define _PSADBW (0x557ULL << 32)
#define _CVTPD2PS (0x571ULL << 32)
#define _PUNPCKLWD (0x588ULL << 32)
#define _PSRLD (0x58aULL << 32)
#define _PSRAD (0x58cULL << 32)
#define _PSLLD (0x58eULL << 32)
#define _CVTTPD2PI (0x594ULL << 32)
#define _PUNPCKHWD (0x598ULL << 32)
/* #define _PSRLD (0x59aULL << 32) */
#define _PSHUFD (0x59bULL << 32)
/* #define _PSRAD (0x59cULL << 32) */
#define _PACKSSWB (0x59dULL << 32)
/* #define _PSLLD (0x59eULL << 32) */
#define _PACKUSWB (0x59fULL << 32)
#define _PUNPCKLBW (0x5c8ULL << 32)
#define _PSRLW (0x5caULL << 32)
#define _PSRAW (0x5ccULL << 32)
#define _PSLLW (0x5ceULL << 32)
#define _PUNPCKHBW (0x5d8ULL << 32)
/* #define _PSRLW (0x5daULL << 32) */
/* #define _PSRAW (0x5dcULL << 32) */
/* #define _PSLLW (0x5deULL << 32) */
#define _CVTDQ2PS (0x5f3ULL << 32)
#define _CVTPI2PD (0x5f5ULL << 32)
#define _MOVUSS (0x539ULL << 32)
#define _SHUFSS (0x53aULL << 32)
#define _MOVUSD (0x579ULL << 32)
#define _SHUFSD (0x57aULL << 32)
#define _MOVUPD (0x5b9ULL << 32)
#define _SHUFPD (0x5baULL << 32)
#define _MOVUPS (0x5f9ULL << 32)
#define _SHUFPS (0x5faULL << 32)
#define _FADDP (0x608ULL << 32)
#define _FDIV (0x646ULL << 32)
#define _PMULUDQ (0x650ULL << 32)
#define _PMULHUW (0x652ULL << 32)
#define _PMULLW (0x654ULL << 32)
#define _PMULHW (0x655ULL << 32)
#define _FILD (0x685ULL << 32)
#define _FCOM2 (0x68aULL << 32)
#define _MULSS (0x631ULL << 32)
#define _SQRTSS (0x635ULL << 32)
#define _DIVSS (0x636ULL << 32)
#define _ADDSS (0x638ULL << 32)
#define _SUBSS (0x63cULL << 32)
#define _MINSS (0x63dULL << 32)
#define _CMPSS (0x63eULL << 32)
#define _MAXSS (0x63fULL << 32)
#define _SQRTSD (0x675ULL << 32)
#define _DIVSD (0x676ULL << 32)
#define _ADDSD (0x678ULL << 32)
#define _SUBSD (0x67cULL << 32)
#define _MINSD (0x67dULL << 32)
#define _CMPSD (0x67eULL << 32)
#define _MAXSD (0x67fULL << 32)
#define _MULPD (0x6b1ULL << 32)
#define _SQRTPD (0x6b5ULL << 32)
#define _DIVPD (0x6b6ULL << 32)
#define _ADDPD (0x6b8ULL << 32)
#define _SUBPD (0x6bcULL << 32)
#define _MINPD (0x6bdULL << 32)
#define _CMPPD (0x6beULL << 32)
#define _MAXPD (0x6bfULL << 32)
#define _MULPS (0x6f1ULL << 32)
#define _SQRTPS (0x6f5ULL << 32)
#define _DIVPS (0x6f6ULL << 32)
#define _ADDPS (0x6f8ULL << 32)
#define _SUBPS (0x6fcULL << 32)
#define _MINPS (0x6fdULL << 32)
#define _CMPPS (0x6feULL << 32)
#define _MAXPS (0x6ffULL << 32)
#define _PINTMOVDI2MM_DSZ32 (0x705ULL << 32)
#define _FPREADROM_DTYPENOP (0x716ULL << 32)
#define _FCOMIP (0x720ULL << 32)
#define _COMISD (0x722ULL << 32)
#define _UCOMISD (0x723ULL << 32)
#define _PINTMOVDTMM2I_DSZ32 (0x72cULL << 32)
#define _PINTMOVDMM2I_DSZ32 (0x72dULL << 32)
#define _PINTMOVDI2MM_DSZ64 (0x745ULL << 32)
#define _PMOVMSKB (0x769ULL << 32)
#define _PINTMOVDTMM2I_DSZ64 (0x76cULL << 32)
#define _PINTMOVDMM2I_DSZ64 (0x76dULL << 32)
#define _PINSRW (0x785ULL << 32)
#define _PEXTRW (0x7adULL << 32)
#define _RSQRTPS (0x7b8ULL << 32)
#define _FCMOVNE (0x7edULL << 32)
#define _MOVMSKPD (0x7f8ULL << 32)
#define _LA2LIN_DSZ32 (0x81fULL << 32)
#define _MOVETOCREG_AND_DSZ64 (0x822ULL << 32)
#define _BTUJB_DIRECT_NOTTAKEN (0x86aULL << 32)
#define _BTUJNB_DIRECT_NOTTAKEN (0x86bULL << 32)
#define _LA2LIN_DSZ64 (0x89fULL << 32)
#define _MOVETOCREG_SHL_DSZ64 (0x8a2ULL << 32)
#define _MOVETOCREG_OR_DSZ64 (0x902ULL << 32)
#define _CMPUJZ_DIRECT_NOTTAKEN (0x928ULL << 32)
#define _CMPUJNZ_DIRECT_NOTTAKEN (0x929ULL << 32)
#define _MOVETOCREG_BTS_DSZ64 (0x962ULL << 32)
#define _MOVETOCREG_SHR_DSZ64 (0x9a2ULL << 32)
#define _SHLD (0x996ULL << 32)
#define _SHRD (0x997ULL << 32)
#define _MOVETOCREG_BTR_DSZ64 (0xa62ULL << 32)
#define _LDZX_DSZ32_ASZ32_SC1 (0xc00ULL << 32)
#define _LEA_DSZ32_ASZ32_SC1 (0xc03ULL << 32)
#define _STAD_DSZ32_ASZ32_SC1 (0xc08ULL << 32)
#define _STADTICKLE_DSZ32_ASZ32_SC1 (0xc09ULL << 32)
#define _LDTICKLE_DSZ32_ASZ32_SC1 (0xc0aULL << 32)
#define _MOVNTPD (0xc0eULL << 32)
/* #define _LDZX_DSZ32_ASZ32_SC1 (0xc10ULL << 32) */
/* #define _LEA_DSZ32_ASZ32_SC1 (0xc13ULL << 32) */
/* #define _STAD_DSZ32_ASZ32_SC1 (0xc18ULL << 32) */
/* #define _STADTICKLE_DSZ32_ASZ32_SC1 (0xc19ULL << 32) */
/* #define _LDTICKLE_DSZ32_ASZ32_SC1 (0xc1aULL << 32) */
/* #define _MOVHPD (0xc2eULL << 32) */
/* #define _LDZX_DSZ32_ASZ32_SC1 (0xc30ULL << 32) */
/* #define _LEA_DSZ32_ASZ32_SC1 (0xc33ULL << 32) */
/* #define _STAD_DSZ32_ASZ32_SC1 (0xc38ULL << 32) */
/* #define _STADTICKLE_DSZ32_ASZ32_SC1 (0xc39ULL << 32) */
/* #define _LDTICKLE_DSZ32_ASZ32_SC1 (0xc3aULL << 32) */
#define _LDZX_DSZ64_ASZ32_SC1 (0xc40ULL << 32)
#define _LEA_DSZ64_ASZ32_SC1 (0xc43ULL << 32)
#define _STAD_DSZ64_ASZ32_SC1 (0xc48ULL << 32)
#define _STADTICKLE_DSZ64_ASZ32_SC1 (0xc49ULL << 32)
#define _LDTICKLE_DSZ64_ASZ32_SC1 (0xc4aULL << 32)
#define _RDSEGFLD (0xc4bULL << 32)
/* #define _LDZX_DSZ64_ASZ32_SC1 (0xc50ULL << 32) */
/* #define _LEA_DSZ64_ASZ32_SC1 (0xc53ULL << 32) */
/* #define _STAD_DSZ64_ASZ32_SC1 (0xc58ULL << 32) */
/* #define _STADTICKLE_DSZ64_ASZ32_SC1 (0xc59ULL << 32) */
/* #define _LDTICKLE_DSZ64_ASZ32_SC1 (0xc5aULL << 32) */
/* #define _MOVLPD (0xc5eULL << 32) */
#define _WRSEGFLD (0xc6bULL << 32)
#define _STAD_DSZ64_ASZ64_SC1 (0xc68ULL << 32)
/* #define _LDZX_DSZ64_ASZ32_SC1 (0xc70ULL << 32) */
/* #define _LEA_DSZ64_ASZ32_SC1 (0xc73ULL << 32) */
/* #define _STAD_DSZ64_ASZ64_SC8 (0xc78ULL << 32) */
/* #define _STADTICKLE_DSZ64_ASZ32_SC1 (0xc79ULL << 32) */
/* #define _LDTICKLE_DSZ64_ASZ32_SC1 (0xc7aULL << 32) */
/* #define _WRSEGFLD (0xc7bULL << 32) */
/* #define _LDZX_DSZ16_ASZ32_SC1 (0xc80ULL << 32) */
#define _LEA_DSZ16_ASZ32_SC1 (0xc83ULL << 32)
#define _STAD_DSZ16_ASZ32_SC1 (0xc88ULL << 32)
#define _STADTICKLE_DSZ16_ASZ32_SC1 (0xc89ULL << 32)
#define _LDTICKLE_DSZ16_ASZ32_SC1 (0xc8aULL << 32)
/* #define _LDZX_DSZ16_ASZ32_SC1 (0xc90ULL << 32) */
/* #define _LEA_DSZ16_ASZ32_SC1 (0xc93ULL << 32) */
/* #define _STAD_DSZ16_ASZ32_SC1 (0xc98ULL << 32) */
/* #define _STADTICKLE_DSZ16_ASZ32_SC1 (0xc99ULL << 32) */
/* #define _LDTICKLE_DSZ16_ASZ32_SC1 (0xc9aULL << 32) */
#define _LDZX_DSZ16_ASZ32_SC1 (0xcb0ULL << 32)
/* #define _LEA_DSZ16_ASZ32_SC1 (0xcb3ULL << 32) */
/* #define _STAD_DSZ16_ASZ32_SC1 (0xcb8ULL << 32) */
/* #define _STADTICKLE_DSZ16_ASZ32_SC1 (0xcb9ULL << 32) */
/* #define _LDTICKLE_DSZ16_ASZ32_SC1 (0xcbaULL << 32) */
#define _MOVNTDQ (0xcbeULL << 32)
#define _LDZX_DSZ8_ASZ32_SC1 (0xcc0ULL << 32)
#define _LEA_DSZ8_ASZ32_SC1 (0xccfULL << 32)
#define _STAD_DSZ8_ASZ32_SC1 (0xcc8ULL << 32)
#define _STADTICKLE_DSZ8_ASZ32_SC1 (0xcc9ULL << 32)
#define _LDTICKLE_DSZ8_ASZ32_SC1 (0xccaULL << 32)
/* #define _LDZX_DSZ8_ASZ32_SC1 (0xcd0ULL << 32) */
/* #define _LEA_DSZ8_ASZ32_SC1 (0xcd3ULL << 32) */
/* #define _STAD_DSZ8_ASZ32_SC1 (0xcd8ULL << 32) */
/* #define _STADTICKLE_DSZ8_ASZ32_SC1 (0xcd9ULL << 32) */
/* #define _LDTICKLE_DSZ8_ASZ32_SC1 (0xcdaULL << 32) */
#define _STAD_DSZ8_ASZ64_SC1 (0xce8ULL << 32)
/* #define _LDZX_DSZ8_ASZ32_SC1 (0xcf0ULL << 32) */
/* #define _LEA_DSZ8_ASZ32_SC1 (0xcf3ULL << 32) */
#define _LDHINT_BUFFER_ASZ32_SC1 (0xcf5ULL << 32)
/* #define _STAD_DSZ8_ASZ32_SC1 (0xcf8ULL << 32) */
/* #define _STADTICKLE_DSZ8_ASZ32_SC1 (0xcf9ULL << 32) */
/* #define _LDTICKLE_DSZ8_ASZ32_SC1 (0xcfaULL << 32) */
#define _MASKMOVDQU (0xcfeULL << 32)
#define _LDZX_DSZ32_ASZ32_SC4 (0xd00ULL << 32)
#define _LEA_DSZ32_ASZ32_SC4 (0xd03ULL << 32)
#define _STAD_DSZ32_ASZ32_SC4 (0xd08ULL << 32)
#define _STADTICKLE_DSZ32_ASZ32_SC4 (0xd09ULL << 32)
#define _LDTICKLE_DSZ32_ASZ32_SC4 (0xd0aULL << 32)
#define _PORTIN_DSZ32_ASZ16_SC1 (0xd0bULL << 32)
#define _PORTOUT_DSZ32_ASZ16_SC1 (0xd0fULL << 32)
/* #define _LDZX_DSZ32_ASZ32_SC4 (0xd10ULL << 32) */
/* #define _LEA_DSZ32_ASZ32_SC4 (0xd13ULL << 32) */
/* #define _STAD_DSZ32_ASZ32_SC4 (0xd18ULL << 32) */
/* #define _STADTICKLE_DSZ32_ASZ32_SC4 (0xd19ULL << 32) */
/* #define _LDTICKLE_DSZ32_ASZ32_SC4 (0xd1aULL << 32) */
/* #define _LDZX_DSZ32_ASZ32_SC1 (0xd30ULL << 32) */
/* #define _LEA_DSZ32_ASZ32_SC1 (0xd33ULL << 32) */
/* #define _STAD_DSZ32_ASZ32_SC1 (0xd38ULL << 32) */
/* #define _STADTICKLE_DSZ32_ASZ32_SC1 (0xd39ULL << 32) */
/* #define _LDTICKLE_DSZ32_ASZ32_SC1 (0xd3aULL << 32) */
#define _LDZX_DSZ64_ASZ32_SC4 (0xd40ULL << 32)
#define _LEA_DSZ64_ASZ32_SC4 (0xd43ULL << 32)
#define _STAD_DSZ64_ASZ32_SC4 (0xd48ULL << 32)
#define _STADTICKLE_DSZ64_ASZ32_SC4 (0xd49ULL << 32)
#define _LDTICKLE_DSZ64_ASZ32_SC4 (0xd4aULL << 32)
#define _PORTIN_DSZ64_ASZ16_SC1 (0xd4bULL << 32)
#define _PORTOUT_DSZ64_ASZ16_SC1 (0xd4fULL << 32)
/* #define _LDZX_DSZ64_ASZ32_SC4 (0xd50ULL << 32) */
/* #define _LEA_DSZ64_ASZ32_SC4 (0xd53ULL << 32) */
/* #define _STAD_DSZ64_ASZ32_SC4 (0xd58ULL << 32) */
/* #define _STADTICKLE_DSZ64_ASZ32_SC4 (0xd59ULL << 32) */
/* #define _LDTICKLE_DSZ64_ASZ32_SC4 (0xd5aULL << 32) */
/* #define _LDZX_DSZ64_ASZ32_SC1 (0xd70ULL << 32) */
/* #define _LEA_DSZ64_ASZ32_SC1 (0xd73ULL << 32) */
#define _STAD_DSZ64_ASZ32_SC8 (0xd78ULL << 32)
/* #define _STADTICKLE_DSZ64_ASZ32_SC1 (0xd79ULL << 32) */
/* #define _LDTICKLE_DSZ64_ASZ32_SC1 (0xd7aULL << 32) */
#define _LDZX_DSZ16_ASZ32_SC4 (0xd80ULL << 32)
#define _LEA_DSZ16_ASZ32_SC4 (0xd83ULL << 32)
#define _STAD_DSZ16_ASZ32_SC4 (0xd88ULL << 32)
#define _STADTICKLE_DSZ16_ASZ32_SC4 (0xd89ULL << 32)
#define _LDTICKLE_DSZ16_ASZ32_SC4 (0xd8aULL << 32)
#define _PORTIN_DSZ16_ASZ16_SC1 (0xd8bULL << 32)
#define _PORTOUT_DSZ16_ASZ16_SC1 (0xd8fULL << 32)
/* #define _LDZX_DSZ16_ASZ32_SC4 (0xd90ULL << 32) */
/* #define _LEA_DSZ16_ASZ32_SC4 (0xd93ULL << 32) */
/* #define _STAD_DSZ16_ASZ32_SC4 (0xd98ULL << 32) */
/* #define _STADTICKLE_DSZ16_ASZ32_SC4 (0xd99ULL << 32) */
/* #define _LDTICKLE_DSZ16_ASZ32_SC4 (0xd9aULL << 32) */
/* #define _LDZX_DSZ16_ASZ32_SC1 (0xdb0ULL << 32) */
/* #define _LEA_DSZ16_ASZ32_SC1 (0xdb3ULL << 32) */
/* #define _STAD_DSZ16_ASZ32_SC1 (0xdb8ULL << 32) */
/* #define _STADTICKLE_DSZ16_ASZ32_SC1 (0xdb9ULL << 32) */
/* #define _LDTICKLE_DSZ16_ASZ32_SC1 (0xdbaULL << 32) */
#define _LDZX_DSZ8_ASZ32_SC4 (0xdc0ULL << 32)
#define _LEA_DSZ8_ASZ32_SC4 (0xdc3ULL << 32)
#define _STAD_DSZ8_ASZ32_SC4 (0xdc8ULL << 32)
#define _STADTICKLE_DSZ8_ASZ32_SC4 (0xdc9ULL << 32)
#define _LDTICKLE_DSZ8_ASZ32_SC4 (0xdcaULL << 32)
#define _PORTIN_DSZ8_ASZ16_SC1 (0xdcbULL << 32)
#define _PORTOUT_DSZ8_ASZ16_SC1 (0xdcfULL << 32)
/* #define _LDZX_DSZ8_ASZ32_SC4 (0xdd0ULL << 32) */
/* #define _LEA_DSZ8_ASZ32_SC4 (0xdd3ULL << 32) */
/* #define _STAD_DSZ8_ASZ32_SC4 (0xdd8ULL << 32) */
/* #define _STADTICKLE_DSZ8_ASZ32_SC4 (0xdd9ULL << 32) */
/* #define _LDTICKLE_DSZ8_ASZ32_SC4 (0xddaULL << 32) */
/* #define _LDZX_DSZ8_ASZ32_SC1 (0xdf0ULL << 32) */
/* #define _LEA_DSZ8_ASZ32_SC1 (0xdf3ULL << 32) */
/* #define _STAD_DSZ8_ASZ32_SC1 (0xdf8ULL << 32) */
/* #define _STADTICKLE_DSZ8_ASZ32_SC1 (0xdf9ULL << 32) */
/* #define _LDTICKLE_DSZ8_ASZ32_SC1 (0xdfaULL << 32) */
#define _LDPPHYS_DSZ32_ASZ16_SC1 (0xe00ULL << 32)
#define _STADPPHYS_DSZ32_ASZ16_SC1 (0xe08ULL << 32)
/* #define _LDPPHYS_DSZ32_ASZ16_SC1 (0xe0aULL << 32) */
#define _STADPPHYSTICKLE_DSZ32_ASZ16_SC1 (0xe0dULL << 32)
#define _LDPPHYS_DSZ32_ASZ64_SC1 (0xe20ULL << 32)
#define _LDPPHYSTICKLE_DSZ32_ASZ64_SC1 (0xe25ULL << 32)
#define _STADPPHYS_DSZ32_ASZ64_SC1 (0xe28ULL << 32)
/* #define _LDPPHYS_DSZ32_ASZ64_SC1 (0xe2aULL << 32) */
#define _STADPPHYSTICKLE_DSZ32_ASZ64_SC1 (0xe2dULL << 32)
#define _SIMDSTADPPHYS_DSZ32_ASZ64_SC1 (0xe2eULL << 32)
#define _LDPPHYS_DSZ32_ASZ64_SC8 (0xe30ULL << 32)
#define _STADPPHYS_DSZ32_ASZ64_SC8 (0xe38ULL << 32)
/* #define _LDPPHYS_DSZ32_ASZ64_SC8 (0xe3aULL << 32) */
#define _LDPPHYS_DSZ64_ASZ16_SC1 (0xe40ULL << 32)
#define _STADPPHYS_DSZ64_ASZ16_SC1 (0xe48ULL << 32)
/* #define _LDPPHYS_DSZ64_ASZ16_SC1 (0xe4aULL << 32) */
#define _STADPPHYSTICKLE_DSZ64_ASZ16_SC1 (0xe4dULL << 32)
#define _LDPPHYS_DSZ64_ASZ64_SC1 (0xe60ULL << 32)
#define _LDPPHYSTICKLE_DSZ64_ASZ64_SC1 (0xe65ULL << 32)
#define _STADPPHYS_DSZ64_ASZ64_SC1 (0xe68ULL << 32)
/* #define _LDPPHYS_DSZ64_ASZ64_SC1 (0xe6aULL << 32) */
#define _STADPPHYSTICKLE_DSZ64_ASZ64_SC1 (0xe6dULL << 32)
#define _SIMDSTADPPHYS_DSZ64_ASZ64_SC1 (0xe6eULL << 32)
#define _LDPPHYS_DSZ64_ASZ64_SC8 (0xe70ULL << 32)
#define _LDSTGBUF_DSZ64_ASZ16_SC1 (0xe75ULL << 32)
#define _STADPPHYS_DSZ64_ASZ64_SC8 (0xe78ULL << 32)
/* #define _LDPPHYS_DSZ64_ASZ64_SC8 (0xe7aULL << 32) */
#define _STADSTGBUF_DSZ64_ASZ16_SC1 (0xe7dULL << 32)
#define _LDPPHYS_DSZ16_ASZ16_SC1 (0xe80ULL << 32)
#define _STADPPHYS_DSZ16_ASZ16_SC1 (0xe88ULL << 32)
/* #define _LDPPHYS_DSZ16_ASZ16_SC1 (0xe8aULL << 32) */
#define _STADPPHYSTICKLE_DSZ16_ASZ16_SC1 (0xe8dULL << 32)
#define _LDPPHYS_DSZ16_ASZ64_SC1 (0xea0ULL << 32)
#define _LDPPHYSTICKLE_DSZ16_ASZ64_SC1 (0xea5ULL << 32)
#define _STADPPHYS_DSZ16_ASZ64_SC1 (0xea8ULL << 32)
/* #define _LDPPHYS_DSZ16_ASZ64_SC1 (0xeaaULL << 32) */
#define _STADPPHYSTICKLE_DSZ16_ASZ64_SC1 (0xeadULL << 32)
#define _SIMDLSTADSTGBUF_DSZ64_ASZ32_SC1 (0xeaeULL << 32)
#define _LDPPHYS_DSZ16_ASZ64_SC8 (0xeb0ULL << 32)
#define _STADPPHYS_DSZ16_ASZ64_SC8 (0xeb8ULL << 32)
/* #define _LDPPHYS_DSZ16_ASZ64_SC8 (0xebaULL << 32) */
#define _LDPPHYS_DSZ8_ASZ16_SC1 (0xec0ULL << 32)
#define _STADPPHYS_DSZ8_ASZ16_SC1 (0xec8ULL << 32)
/* #define _LDPPHYS_DSZ8_ASZ16_SC1 (0xecaULL << 32) */
#define _LDHINT_CACHEALL_ASZ64_SC1 (0xecbULL << 32)
#define _STADPPHYSTICKLE_DSZ8_ASZ16_SC1 (0xecdULL << 32)
#define _LDHINT_CACHE1_ASZ64_SC1 (0xedbULL << 32)
#define _LDPPHYS_DSZ8_ASZ64_SC1 (0xee0ULL << 32)
#define _LDPPHYSTICKLE_DSZ8_ASZ64_SC1 (0xee5ULL << 32)
#define _STADPPHYS_DSZ8_ASZ64_SC1 (0xee8ULL << 32)
/* #define _LDPPHYS_DSZ8_ASZ64_SC1 (0xeeaULL << 32) */
#define _LDHINT_CACHE2_ASZ64_SC1 (0xeebULL << 32)
#define _STADPPHYSTICKLE_DSZ8_ASZ64_SC1 (0xeedULL << 32)
#define _SIMDHSTADSTGBUF_DSZ64_ASZ32_SC1 (0xeeeULL << 32)
#define _LDPPHYS_DSZ8_ASZ64_SC8 (0xef0ULL << 32)
#define _STADPPHYS_DSZ8_ASZ64_SC8 (0xef8ULL << 32)
/* #define _LDPPHYS_DSZ8_ASZ64_SC8 (0xefaULL << 32) */
#define _LDPPHYS_DSZ32_ASZ32_SC4 (0xf00ULL << 32)
#define _STADPPHYS_DSZ32_ASZ32_SC4 (0xf08ULL << 32)
/* #define _LDPPHYS_DSZ32_ASZ32_SC4 (0xf0aULL << 32) */
#define _LDPPHYS_DSZ32_ASZ32_SC1 (0xf20ULL << 32)
#define _STADPPHYS_DSZ32_ASZ32_SC1 (0xf28ULL << 32)
/* #define _LDPPHYS_DSZ32_ASZ32_SC1 (0xf2aULL << 32) */
#define _LDPPHYS_DSZ32_ASZ32_SC8 (0xf30ULL << 32)
#define _STADPPHYS_DSZ32_ASZ32_SC8 (0xf38ULL << 32)
/* #define _LDPPHYS_DSZ32_ASZ32_SC8 (0xf3aULL << 32) */
#define _LDPPHYS_DSZ64_ASZ32_SC4 (0xf40ULL << 32)
#define _STADPPHYS_DSZ64_ASZ32_SC4 (0xf48ULL << 32)
/* #define _LDPPHYS_DSZ64_ASZ32_SC4 (0xf4aULL << 32) */
#define _MPOP (0xf4bULL << 32)
#define _MPUSH (0xf4fULL << 32)
#define _LDPPHYS_DSZ64_ASZ32_SC1 (0xf60ULL << 32)
#define _LDPPHYSTICKLE_DSZ64_ASZ32_SC1 (0xf65ULL << 32)
#define _STADPPHYS_DSZ64_ASZ32_SC1 (0xf68ULL << 32)
/* #define _LDPPHYS_DSZ64_ASZ32_SC1 (0xf6aULL << 32) */
#define _MCALL_DIRECT (0xf6fULL << 32)
#define _LDPPHYS_DSZ64_ASZ32_SC8 (0xf70ULL << 32)
#define _STADPPHYS_DSZ64_ASZ32_SC8 (0xf78ULL << 32)
/* #define _LDPPHYS_DSZ64_ASZ32_SC8 (0xf7aULL << 32) */
#define _UCALLPARAM_INDIRECT (0xf7fULL << 32)
#define _LDPPHYS_DSZ16_ASZ32_SC4 (0xf80ULL << 32)
#define _STADPPHYS_DSZ16_ASZ32_SC4 (0xf88ULL << 32)
/* #define _LDPPHYS_DSZ16_ASZ32_SC4 (0xf8aULL << 32) */
#define _LDPPHYS_DSZ16_ASZ32_SC1 (0xfa0ULL << 32)
#define _STADPPHYS_DSZ16_ASZ32_SC1 (0xfa8ULL << 32)
/* #define _LDPPHYS_DSZ16_ASZ32_SC1 (0xfaaULL << 32) */
#define _LDPPHYS_DSZ16_ASZ32_SC8 (0xfb0ULL << 32)
#define _STADPPHYS_DSZ16_ASZ32_SC8 (0xfb8ULL << 32)
/* #define _LDPPHYS_DSZ16_ASZ32_SC8 (0xfbaULL << 32) */
#define _LDPPHYS_DSZ8_ASZ32_SC4 (0xfc0ULL << 32)
#define _STADPPHYS_DSZ8_ASZ32_SC4 (0xfc8ULL << 32)
/* #define _LDPPHYS_DSZ8_ASZ32_SC4 (0xfcaULL << 32) */
#define _LDPPHYS_DSZ8_ASZ32_SC1 (0xfe0ULL << 32)
#define _STADPPHYS_DSZ8_ASZ32_SC1 (0xfe8ULL << 32)
/* #define _LDPPHYS_DSZ8_ASZ32_SC1 (0xfeaULL << 32) */
#define _LBSYNC (0xfefULL << 32)
#define _LDPPHYS_DSZ8_ASZ32_SC8 (0xff0ULL << 32)
#define _STADPPHYS_DSZ8_ASZ32_SC8 (0xff8ULL << 32)
/* #define _LDPPHYS_DSZ8_ASZ32_SC8 (0xffaULL << 32) */
#define _SFENCE (0xfffULL << 32)

#define _PINTMOVQI2XMMHQ_DSZ64 (0x746ULL << 32)
#define _PINTMOVQI2XMMLQ_DSZ64 (0x747ULL << 32)
#define _PINTMOVQXMMLQ2I_DSZ64 (0x76fULL << 32)

#endif // OPCODE_H_
