ARCH = $(shell uname -m)
ifneq ("$(ARCH)", "ppc64")
ifneq ("$(ARCH)", "ppc64le")
	CROSS_COMPILE ?= powerpc64le-linux-gnu-
endif
endif

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = -Os -g -Wall -std=c99 -msoft-float -mno-string -mno-multiple -mno-vsx -mno-altivec -mlittle-endian -fno-stack-protector -mstrict-align -ffreestanding -fdata-sections -ffunction-sections -I../include
ASFLAGS = $(CFLAGS)
LDFLAGS = -T powerpc.lds

all: read_test.hex

console.o: ../lib/console.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

read_test.elf: read_test.o head.o console.o
	$(LD) $(LDFLAGS) -o $@ $^

read_test.bin: read_test.elf
	$(OBJCOPY) -O binary $^ $@

read_test.hex: read_test.bin
	../scripts/bin2hex.py $^ > $@

clean:
	@rm -f *.o read_test.elf read_test.bin read_test.hex
distclean: clean
	rm -f *~

