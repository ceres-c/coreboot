/*
 * Federico (ceres-c) Cerutti
 */

#include <libpayload.h>
#include <exception.h>
#include <arch/apic.h>
#include "payload.h"

bool msr_exists = true;

struct intr_gate { /* Copied from coreboot source */
	uint16_t offset_0;
	uint16_t segsel;
	uint16_t flags;
	uint16_t offset_1;
#if ENV_X86_64
	uint32_t offset_2;
	uint32_t reserved;
#endif
} __packed;
extern struct intr_gate idt[] __aligned(8);

void vec13_msr_handler(void) {
__asm__(
    "addl    $2, 4(%esp)\n" /* skip the instruction that caused the exception (EIP points to rdmrs itself) */
    "addl    $4, %esp\n"    /* pop of the error code */
    "movl    $0, msr_exists\n"
    "iret\n"
);
}

int main(void)
{
	/* Install our exception handler */
	idt[13].offset_0 = (uintptr_t)vec13_msr_handler;
	idt[13].offset_1 = (uintptr_t)vec13_msr_handler >> 16;
    printf("Registered handler for %d\n", EXC_GP);

    while (1) {
        msr_exists = true;
        char* input = readline("Give me some text (`E` to end): ");
        if (strlen(input) == 1 && *input == 'E') break;
        printf("You gave me: %s\n", input);

        unsigned int low = 0, high = 0;
        __asm__ volatile ("rdmsr" : "=a" (low), "=d" (high) : "c" (APL_UCODE_CRBUS_UNLOCK));
        if (!msr_exists) {
            printf("Error reading msr 0x%x\n", APL_UCODE_CRBUS_UNLOCK);
        } else {
            printf("MSR value: 0x%x%x\n", high, low);
        }

        register uint32_t eax asm("eax");
        __asm__ __volatile__(
            ".byte 0x0f, 0xc7, 0xf0;"); // rdrand eax - otherwise gcc gives `operand size mismatch`
        printf("rdrand eax: 0x%08x\n", eax);
    }
    puts("Now we will halt. Bye");
    halt();
    return 0;
}
