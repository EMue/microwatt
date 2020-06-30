#include <stdbool.h>
#include <stdint.h>

#include "console.h"

#include "../include/io.h"
#include "../include/microwatt_soc.h"

#define DATA_STEP 15597894987294837521UL
#define PRINT_STEP (1 << 20)
#define SEGMENT_STEP (1 << 24)

static void print_val(unsigned long val, int digits)
{
	for(int i = 0; i < digits; i++) {
		unsigned char c = (val >> (digits - i - 1) * 4) & 0xf;
		if(c < 10)
			c += '0';
		else
			c += 'a' - 10;
		putchar(c);
	}
}

static bool write(unsigned long addr, unsigned long data)
{
	writeq(data, addr);
	return true;
}

static bool check(unsigned long addr, unsigned long data)
{
	unsigned long read = readq(addr);
	if(read != data) {
		puts("Wrong value at ");
		print_val(addr, 8);
		puts(": ");
		print_val(read, 16);
		puts(", expected ");
		print_val(data, 16);
		return false;
	} else
		return true;
}

static void process_segment(
		unsigned long start, unsigned long stop,
		bool (*fn)(unsigned long addr, unsigned long data))
{
	unsigned long data_val = DATA_STEP;
	for(unsigned long pos = start;
	    pos < stop;
	    pos += sizeof(unsigned long)) {
		if(!fn(pos, data_val))
			return;
		if(!(pos % PRINT_STEP)) {
			print_val(pos, 8);
			puts(": ");
			print_val(data_val, 16);
			puts("\r");
		}
		data_val += DATA_STEP;
		data_val %= (1UL << 63) - 1;
	}
}

static void check_segment(unsigned long addr, unsigned long len)
{
	puts("Writing ");
	print_val(addr, 8);
	puts("-");
	print_val(addr + len, 8);
	puts("...\n\r");

	process_segment(addr, addr + len, write);
	puts("\n\r");

	puts("Checking ");
	print_val(addr, 8);
	puts("-");
	print_val(addr + len, 8);
	puts("...\n\r");

	process_segment(addr, addr + len, check);
	puts("\n\r");
}

int main(void)
{
	console_init();

	check_segment(BRAM_BASE + 0x2000,
			(readq(SYSCON_BASE + SYS_REG_BRAMINFO)
			 & SYS_REG_BRAMINFO_SIZE_MASK) - 0x2000);

	unsigned long dram_len =
		readq(SYSCON_BASE + SYS_REG_DRAMINFO)
		& SYS_REG_DRAMINFO_SIZE_MASK;
	unsigned long dram_max = DRAM_BASE + dram_len;
	for(unsigned long start = DRAM_BASE;
	    start < dram_max;
	   ) {
		unsigned long stop = start + SEGMENT_STEP;
		if(stop > dram_max)
			stop = dram_max;
		check_segment(start, stop - start);
		start = stop;
	}

	puts("Done!");

	return 0;
}
