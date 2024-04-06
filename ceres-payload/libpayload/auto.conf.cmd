deps_config := \
	arch/mock/Kconfig \
	arch/x86/Kconfig \
	arch/arm64/Kconfig \
	arch/arm/Kconfig \
	drivers/usb/Kconfig \
	drivers/storage/Kconfig \
	drivers/timer/Kconfig \
	vboot/Kconfig \
	libcbfs/Kconfig \
	Kconfig \

/root/coreboot/ceres-payload/libpayload/auto.conf: $(deps_config)


$(deps_config): ;
