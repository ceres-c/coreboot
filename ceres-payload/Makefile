unexport $(COREBOOT_EXPORTS)

BUILD_NAME = payload

ARCH = x86_32
OBJDIR = build
OBJS = $(OBJDIR)/$(BUILD_NAME).o
TARGET = $(OBJDIR)/$(BUILD_NAME).elf

all: $(TARGET)

include ../payloads/libpayload/Makefile.payload

.PHONY: all
