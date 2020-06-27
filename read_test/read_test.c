#include <stdbool.h>
#include <stdint.h>

#include "console.h"

#include "../include/io.h"
#include "../include/microwatt_soc.h"

#define START_ADDR 0x1000U
#define STOP_ADDR 0x1600U

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

static void print_range(unsigned long base)
{
	for(unsigned long addr = base + START_ADDR;
	    addr < base + STOP_ADDR;
	    )
	{
		print_val(addr, 8);
		puts(": ");
		for(int i = 0; i < 4; i++) {
			print_val(readq(addr), 16);
			addr += sizeof(unsigned long);
			putchar(' ');
		}
		puts("\n\r");
	}
}

int main(void)
{
	console_init();

	puts("From block RAM (");
	print_val(BRAM_BASE, 8);
	puts("), should succeed:\n\r");
	print_range(BRAM_BASE);
	puts("OK.\n\r");

	puts("From aliased memory (");
	print_val(MEMORY_BASE, 8);
	puts("), might lock up:\n\r");
	print_range(MEMORY_BASE);
	puts("OK.\n\r");

	return 0;
}
