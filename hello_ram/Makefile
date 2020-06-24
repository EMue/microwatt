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

all: hello_ram.hex

console.o: ../lib/console.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

hello_ram.elf: hello_ram.o head.o console.o
	$(LD) $(LDFLAGS) -o $@ $^

hello_ram.bin: hello_ram.elf
	$(OBJCOPY) -O binary $^ $@

hello_ram.hex: hello_ram.bin
	../scripts/bin2hex.py $^ > $@

clean:
	@rm -f *.o hello_ram.elf hello_ram.bin hello_ram.hex
distclean: clean
	rm -f *~

