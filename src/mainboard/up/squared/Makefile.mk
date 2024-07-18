## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

romstage-y += romstage.c

ramstage-y += ramstage.c
ramstage-$(CONFIG_RED_UNLOCK) += red_unlock.c
# cbfs-files-$(CONFIG_RED_UNLOCK) += ucode_report.bin # Add report file to cbfs
# ifeq ($(CONFIG_RED_UNLOCK),y)
# cpu_microcode_bins = $(wildcard ucode_tests/06-5c-0a) # Overwrite other ucode with only relevant file
# endif

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
