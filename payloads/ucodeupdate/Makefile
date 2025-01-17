unexport $(COREBOOT_EXPORTS)

ARCH = x86_32
OBJS = $(obj)/ucodeupdate.o
TARGET = $(obj)/ucodeupdate.elf

all: $(TARGET)

include ../libpayload/Makefile.payload

.PHONY: all
