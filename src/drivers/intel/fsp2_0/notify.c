/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/null_breakpoint.h>
#include <bootstate.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <fsp/util.h>
#include <mode_switch.h>
#include <timestamp.h>
#include <types.h>

struct fsp_notify_phase_data {
	enum fsp_notify_phase notify_phase;
	bool skip;
	uint8_t post_code_before;
	uint8_t post_code_after;
	enum timestamp_id timestamp_before;
	enum timestamp_id timestamp_after;
};

bool msr_not_exists = 0;

static const struct fsp_notify_phase_data notify_data[] = {
	{
		.notify_phase     = AFTER_PCI_ENUM,
		.skip             = !CONFIG(USE_FSP_NOTIFY_PHASE_POST_PCI_ENUM),
		.post_code_before = POSTCODE_FSP_NOTIFY_BEFORE_ENUMERATE,
		.post_code_after  = POSTCODE_FSP_NOTIFY_AFTER_ENUMERATE,
		.timestamp_before = TS_FSP_ENUMERATE_START,
		.timestamp_after  = TS_FSP_ENUMERATE_END,
	},
	{
		.notify_phase     = READY_TO_BOOT,
		.skip             = !CONFIG(USE_FSP_NOTIFY_PHASE_READY_TO_BOOT),
		.post_code_before = POSTCODE_FSP_NOTIFY_BEFORE_FINALIZE,
		.post_code_after  = POSTCODE_FSP_NOTIFY_AFTER_FINALIZE,
		.timestamp_before = TS_FSP_FINALIZE_START,
		.timestamp_after  = TS_FSP_FINALIZE_END,
	},
	{
		.notify_phase     = END_OF_FIRMWARE,
		.skip             = !CONFIG(USE_FSP_NOTIFY_PHASE_END_OF_FIRMWARE),
		.post_code_before = POSTCODE_FSP_NOTIFY_BEFORE_END_OF_FIRMWARE,
		.post_code_after  = POSTCODE_FSP_NOTIFY_AFTER_END_OF_FIRMWARE,
		.timestamp_before = TS_FSP_END_OF_FIRMWARE_START,
		.timestamp_after  = TS_FSP_END_OF_FIRMWARE_END,
	},
};

static const struct fsp_notify_phase_data *get_notify_phase_data(enum fsp_notify_phase phase)
{
	for (size_t i = 0; i < ARRAY_SIZE(notify_data); i++) {
		if (notify_data[i].notify_phase == phase)
			return &notify_data[i];
	}
	die("Unknown FSP notify phase %u\n", phase);
}

static void fsp_notify(enum fsp_notify_phase phase)
{
	const struct fsp_notify_phase_data *data = get_notify_phase_data(phase);
	struct fsp_notify_params notify_params = { .phase = phase };
	fsp_notify_fn fspnotify;
	uint32_t ret;

	if (data->skip) {
		printk(BIOS_INFO, "coreboot skipped calling FSP notify phase: %08x.\n", phase);
		return;
	}

	if (!fsps_hdr.notify_phase_entry_offset)
		die("Notify_phase_entry_offset is zero!\n");

	fspnotify = (void *)(uintptr_t)(fsps_hdr.image_base +
				fsps_hdr.notify_phase_entry_offset);
	fsp_before_debug_notify(fspnotify, &notify_params);

	timestamp_add_now(data->timestamp_before);
	post_code(data->post_code_before);

	if (data->post_code_after == POSTCODE_FSP_NOTIFY_AFTER_END_OF_FIRMWARE) {
		// This is the step that can be skipped in CSE init if we can read the magic MSR

		// struct intr_gate backup = {
		// 	.offset_0 = idt[13].offset_0,
		// 	.segsel = idt[13].segsel,
		// 	.flags = idt[13].flags,
		// 	.offset_1 = idt[13].offset_1,
		// #if ENV_X86_64
		// 	.offset_2 = idt[13].offset_2,
		// 	.reserved = idt[13].reserved,
		// #endif
		// };

		// static const uintptr_t vec13_msr_handler_ptr = (uintptr_t)vec13_msr_handler;
		// idt[13].offset_0 = vec13_msr_handler_ptr;
		// idt[13].offset_1 = vec13_msr_handler_ptr >> 16;

		// addr:
		// printk(BIOS_INFO, "INT13: in caller\n\tCurrent addr %p\n\tvec13_msr_handler_ptr: 0x%lx\n", &&addr, vec13_msr_handler_ptr);
		// printk(BIOS_INFO, "orig offset_0 0x%x\norig offset_1 0x%x\n", backup.offset_0, backup.offset_1);
		// printk(BIOS_INFO, "new offset_0  0x%x\nnew offset_1  0x%x\n", idt[13].offset_0, idt[13].offset_1);
		// printk(BIOS_INFO, "local_arr[0] 0x%lx", intr_entries[0]);

		#define STR_HELPER(x) #x
		#define STR(x) STR_HELPER(x)

		uint32_t low = 0, high = 0; // These variables will be populated with garbage if the MSR doesn't exist
		__asm__ volatile ("rdmsr" : "=a" (low), "=d" (high) : "c" (APL_UCODE_CRBUS_UNLOCK));
		if (msr_not_exists) { // Exception handler sets this flag
			printk(BIOS_INFO, "[MSR] " STR(APL_UCODE_CRBUS_UNLOCK) " doesn't exist :(\n");
		} else {
			printk(BIOS_INFO, "[MSR] " STR(APL_UCODE_CRBUS_UNLOCK) " found! Red unlocked already :)\n");
		}

		// idt[13].offset_0 = backup.offset_0;
		// idt[13].offset_1 = backup.offset_1;

		// // __asm__ volatile ("rdmsr" : "=a" (low), "=d" (high) : "c" (msr));
		// // printk(BIOS_INFO, "[MSR]\n\t0x%x value: 0x%x%x\n\nmsr_not_exists: %d", msr, high, low, msr_not_exists);
	}

	/* FSP disables the interrupt handler so remove debug exceptions temporarily  */
	null_breakpoint_disable();
	if (ENV_X86_64 && CONFIG(PLATFORM_USES_FSP2_X86_32))
		ret = protected_mode_call_1arg(fspnotify, (uintptr_t)&notify_params);
	else
		ret = fspnotify(&notify_params);
	null_breakpoint_init();

	timestamp_add_now(data->timestamp_after);
	post_code(data->post_code_after);

	fsp_debug_after_notify(ret);

	/* Handle any errors returned by FspNotify */
	fsp_handle_reset(ret);
	if (ret != FSP_SUCCESS)
		die("FspNotify returned with error 0x%08x!\n", ret);

	/* Allow the platform to run something after FspNotify */
	platform_fsp_notify_status(phase);
}

static void fsp_notify_dummy(void *arg)
{
	enum fsp_notify_phase phase = (uint32_t)(uintptr_t)arg;

	display_mtrrs();

	fsp_notify(phase);
	if (phase == READY_TO_BOOT)
		fsp_notify(END_OF_FIRMWARE);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fsp_notify_dummy, (void *)AFTER_PCI_ENUM);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, fsp_notify_dummy, (void *)READY_TO_BOOT);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, fsp_notify_dummy, (void *)READY_TO_BOOT);

__weak void platform_fsp_notify_status(enum fsp_notify_phase phase)
{
}
