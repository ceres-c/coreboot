/*
 * Loosely inspired (read: copied) from linux's msr.h
 */

// From: linux/arch/x86/include/asm/asm.h
# define DEFINE_EXTABLE_TYPE_REG \
	".macro extable_type_reg type:req reg:req\n"						\
	".set .Lfound, 0\n"									\
	".set .Lregnr, 0\n"									\
	".irp rs,rax,rcx,rdx,rbx,rsp,rbp,rsi,rdi,r8,r9,r10,r11,r12,r13,r14,r15\n"		\
	".ifc \\reg, %%\\rs\n"									\
	".set .Lfound, .Lfound+1\n"								\
	".long \\type + (.Lregnr << 8)\n"							\
	".endif\n"										\
	".set .Lregnr, .Lregnr+1\n"								\
	".endr\n"										\
	".set .Lregnr, 0\n"									\
	".irp rs,eax,ecx,edx,ebx,esp,ebp,esi,edi,r8d,r9d,r10d,r11d,r12d,r13d,r14d,r15d\n"	\
	".ifc \\reg, %%\\rs\n"									\
	".set .Lfound, .Lfound+1\n"								\
	".long \\type + (.Lregnr << 8)\n"							\
	".endif\n"										\
	".set .Lregnr, .Lregnr+1\n"								\
	".endr\n"										\
	".if (.Lfound != 1)\n"									\
	".error \"extable_type_reg: bad register argument\"\n"					\
	".endif\n"										\
	".endm\n"
# define UNDEFINE_EXTABLE_TYPE_REG \
	".purgem extable_type_reg\n"

// From: linux/arch/x86/include/asm/extable_fixup_types.h
#define	EX_TYPE_RDMSR_SAFE		11 /* reg := -EIO */

# define _ASM_EXTABLE_TYPE_REG(from, to, type, reg)				\
	" .pushsection \"__ex_table\",\"a\"\n"					\
	" .balign 4\n"								\
	" .long (" #from ") - .\n"						\
	" .long (" #to ") - .\n"						\
	DEFINE_EXTABLE_TYPE_REG							\
	"extable_type_reg reg=" __stringify(reg) ", type=" __stringify(type) " \n"\
	UNDEFINE_EXTABLE_TYPE_REG						\
	" .popsection\n"

/*
 * both i386 and x86_64 returns 64-bit value in edx:eax, but gcc's "A"
 * constraint has different meanings. For i386, "A" means exactly
 * edx:eax, while for x86_64 it doesn't mean rdx:rax or edx:eax. Instead,
 * it means rax *or* rdx.
 */
#define DECLARE_ARGS(val, low, high)	unsigned long long val
#define EAX_EDX_VAL(val, low, high)	(val)
#define EAX_EDX_RET(val, low, high)	"=A" (val)



# define _ASM_EXTABLE_TYPE(from, to, type)			\
	.pushsection "__ex_table","a" ;				\
	.balign 4 ;						\
	.long (from) - . ;					\
	.long (to) - . ;					\
	.long type ;						\
	.popsection


static inline unsigned long long native_read_msr_safe(unsigned int msr, int *err)
{
	// DECLARE_ARGS(val, low, high);

	// asm volatile("1: rdmsr ; xor %[err],%[err]\n"
	// 	     "2:\n\t"
	// 	     _ASM_EXTABLE_TYPE_REG(1b, 2b, EX_TYPE_RDMSR_SAFE, %[err])
	// 	     : [err] "=r" (*err), EAX_EDX_RET(val, low, high)
	// 	     : "c" (msr));
	// return EAX_EDX_VAL(val, low, high);

	DECLARE_ARGS(val, low, high);

	asm volatile("1: rdmsr\n"
		     "2:\n"
		     _ASM_EXTABLE_TYPE(1b, 2b, EX_TYPE_RDMSR)
		     : EAX_EDX_RET(val, low, high) : "c" (msr));

	return EAX_EDX_VAL(val, low, high);
}
