unexport $(COREBOOT_EXPORTS)

ARCH = x86_32
OBJS = $(obj)/ucodeupdate.o
TARGET = $(obj)/ucodeupdate.elf

all: real-all

include ../libpayload/Makefile.payload

real-all: $(TARGET)

.PHONY: all real-all
