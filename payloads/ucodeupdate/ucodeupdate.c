/*
 *
 * Copyright (C) 2025 Federico (ceres-c) Cerutti
 *
 */

#include <coreboot_tables.h>
#include <libpayload.h>

// #include <curses.h>
// #include <form.h>
// #include <menu.h>

#ifndef HOSTED
#define HOSTED 0
#endif

int main(void)
{
	uint32_t sp; asm volatile ("mov %%esp, %0" : "=r" (sp));
	printf("Stack pointer: 0x%08x\n", sp);
	while (1) {}
}
