/*
 * Federico (ceres-c) Cerutti
 */

#include <libpayload.h>
#include <exception.h>
#include <arch/apic.h>
#include "payload.h"

extern struct sysinfo_t lib_sysinfo;
bool red_unlocked = true;

static void unreadable_msr_handler(u8 vector) {
    printf("aaa %d", vector);
    red_unlocked = false;
    // apic_eoi(vector);
}

int main(void)
{
    // uint32_t i = 0;
    // while (1) {
    // 	i++;
    // 	if (i % 100 == 0) {
    // 		printf("%d ", i);
    // 	}
    // 	volatile int a = 10, b = 5;
    // 	volatile int result;
    // 	asm volatile (
    // 		"imul %[b], %[result]"  // Multiply 'a' by 'b' and store the result in 'result'
    // 		: [result] "=r" (result)  // Output operand: result
    // 		: [a] "0" (a), [b] "r" (b)  // Input operands: a (operand 0) and b (operand 1)
    // 		: // No clobbered registers
    // 	);

    // }

    set_interrupt_handler(EXC_GP, &unreadable_msr_handler);
    printf("Registered handler for %d\n", EXC_GP);

    while (1) {
        red_unlocked = true;
        char* input = readline("Give me some text (`E` to end): ");
        if (strlen(input) == 1 && *input == 'E') break;
        printf("You gave me: %s\n", input);

        int rdmsr_err = 0;
        // rdmsr_val = native_read_msr_safe(REDUNLOCK_MSR, &rdmsr_err);
        unsigned int msr = 0x1e6;
        unsigned int low = 0, high = 0;
        __asm__ volatile ("rdmsr" : "=a" (low), "=d" (high) : "c" (msr));
        if (!red_unlocked) {
            printf("Error reading msr %d\n", rdmsr_err);
        } else {
            printf("MSR value: 0x%x%x\n", high, low);
        }
    }
    // buts(" ... if you did not see printf works, then you have a printf issue\n");
    // printf("Number of memory ranges: %d\n", lib_sysinfo.n_memranges);
    // for (i = 0; i < lib_sysinfo.n_memranges; i++) {
    // 	printf("%d: base 0x%08llx size 0x%08llx type 0x%x\n", i, lib_sysinfo.memrange[i].base, lib_sysinfo.memrange[i].size, lib_sysinfo.memrange[i].type);
    // }
    puts("Now we will halt. Bye");
    halt();
    return 0;
}
