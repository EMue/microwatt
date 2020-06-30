ARCH = $(shell uname -m)
ifneq ("$(ARCH)", "ppc64")
ifneq ("$(ARCH)", "ppc64le")
	CROSS_COMPILE ?= powerpc64le-linux-gnu-
endif
endif

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = -O3 -g -Wall -std=c99 -msoft-float -mno-string -mno-multiple -mno-vsx -mno-altivec -mlittle-endian -fno-stack-protector -mstrict-align -ffreestanding -fdata-sections -ffunction-sections -I../include
ASFLAGS = $(CFLAGS)
LDFLAGS = -T powerpc.lds

all: ram_check.hex

console.o: ../lib/console.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

ram_check.elf: ram_check.o head.o console.o
	$(LD) $(LDFLAGS) -o $@ $^

ram_check.bin: ram_check.elf
	$(OBJCOPY) -O binary $^ $@

ram_check.hex: ram_check.bin
	../scripts/bin2hex.py $^ > $@

clean:
	@rm -f *.o ram_check.elf ram_check.bin ram_check.hex
distclean: clean
	rm -f *~

