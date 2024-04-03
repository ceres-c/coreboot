/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _ARCH_EXCEPTION_H
#define _ARCH_EXCEPTION_H

#include <arch/cpu.h>
#include <commonlib/helpers.h>
#include <stdint.h>

#if (!CONFIG(GDB_STUB))

struct intr_gate {
	uint16_t offset_0;
	uint16_t segsel;
	uint16_t flags;
	uint16_t offset_1;
#if ENV_X86_64
	uint32_t offset_2;
	uint32_t reserved;
#endif
} __packed;

/* Even though the vecX symbols are interrupt entry points just treat them
   like data to more easily get the pointer values in C. Because IDT entries
   format splits the offset field up, one can't use the linker to resolve
   parts of a relocation on x86 ABI. An array of pointers is used to gather
   the symbols. The IDT is initialized at runtime when exception_init() is
   called. */
extern u8 vec0[], vec1[], vec2[], vec3[], vec4[], vec5[], vec6[], vec7[];
extern u8 vec8[], vec9[], vec10[], vec11[], vec12[], vec13[], vec14[], vec15[];
extern u8 vec16[], vec17[], vec18[], vec19[], vec13_msr_handler[];

extern uintptr_t intr_entries[];

extern struct intr_gate idt[] __aligned(8);

extern bool msr_exists;

#endif

#if CONFIG(IDT_IN_EVERY_STAGE) || ENV_RAMSTAGE
asmlinkage void exception_init(void);
#else
static inline void exception_init(void) { /* not implemented */ }
#endif

#endif
