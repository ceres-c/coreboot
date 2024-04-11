#ifndef XLAT_H_
#define XLAT_H_

/* Taken from lib-micro/tools/main/main.c */
#define CPUID_XLAT					0x0be0
#define PAUSE_XLAT					0x0bf0
#define IRET_XLAT					0x07c8 //think this is wrong
#define VMXON_XLAT					0x0ae8
#define VMXOFF_XLAT					0x08c8
#define VMLAUNCH_XLAT				0x0328
#define VMCLEAR_XLAT				0x0af8
#define HLT_XLAT					0x0818
#define SYSEXITQ_XLAT				0x0740
/* Taken from uCodeDisasm/glm_ucode_disasm/lables.txt */
#define SIDT_XLAT					0x02d8
#define VMRESUME_XLAT				0x0320
#define VMLAUNCH_XLAT				0x0328
#define VMWRITE_R64_MEM_XLAT		0x0330
#define RDRAND_XLAT					0x0428
#define RDSEED_XLAT					0x0430
#define UDBGWR_XLAT					0x0660
#define SLDT_M16_XLAT				0x0720
#define SYSEXIT_XLAT				0x0738
#define RDTSCP_XLAT					0x0788
#define HLT_XLAT					0x0818
#define PCOMMIT_XLAT				0x0858
#define MOV_CR0_R64_XLAT			0x0890
#define RSM_XLAT					0x08c0
#define VMXOFF_XLAT					0x08c8
#define ENCLS_XLAT					0x08d0
#define MONITOR_XLAT				0x09f8
#define SLDT_R16_XLAT				0x0a68
#define RDMSR_XLAT					0x0ae0
#define VMXON_XLAT					0x0ae8
#define VMPTRLD_XLAT				0x0af0
#define VMCLEAR_XLAT				0x0af8
#define VMCALL_XLAT					0x0b08
#define ENCLU_XLAT					0x0b10
#define MCHECKRET_XLAT				0x0b50
#define UDBGRD_XLAT					0x0b58
#define LIDT_XLAT					0x0b90
#define STR_M16_XLAT				0x0ba0
#define ACQUIRE_IPC_MUTEX			0x0bc9
#define WMPTRST_XLAT				0x0bd0
#define CPUID_XLAT					0x0be0
#define PAUSE_XLAT					0x0bf0
#define SGDT_XLAT					0x0c10
#define MOV_R64_CR8_XLAT			0x0c70
#define WRMSR_XLAT					0x0c80
#define RDTSC_XLAT					0x0ca8
#define RDPMC_XLAT					0x0cc0
#define VMWRITE_R64_R64_XLAT		0x0cd8

#endif // XLAT_H_