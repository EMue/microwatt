#include <stdint.h>
#include <stdbool.h>

#include "console.h"

#include "../include/io.h"
#include "../include/microwatt_soc.h"

#define HELLO_WORLD "Hello RAM\n"

static void print_val(unsigned long val)
{
	for(int i = 0; i < sizeof(unsigned long) * 2; i++) {
		unsigned char c = val >> (8 * sizeof(unsigned long) - 4);
		if(c < 10)
			c += '0';
		else
			c += 'a' - 10;
		val <<= 4;
		putchar(c);
	}
}

int main(void)
{
	console_init();

	puts(HELLO_WORLD);

	unsigned long ctr = 0;
	unsigned long addr = MEMORY_BASE;
	bool write = false;
	bool dram_at_0 = readq(SYSCON_BASE + SYS_REG_CTRL) & 1;
	while (1) {
		char c = getchar();
		switch(c) {
			case 'm':
				puts("Main memory (");
				puts(dram_at_0 ? "DRAM" : "BRAM");
				puts(")\n\r");
				addr = MEMORY_BASE;
				break;
			case 'b':
				puts("BRAM\n\r");
				addr = BRAM_BASE;
				break;
			case 'd':
				puts("DRAM\n\r");
				addr = DRAM_BASE;
				break;
			case 's':
				puts("Slave application\n\r");
				addr = DRAM_BASE + 0x1ff00000UL;
				break;
			case 'r':
				puts("read\n\r");
				write = false;
				break;
			case 'w':
				puts("read, then write\n\r");
				write = true;
				break;
			default:
				for(int i = 0; i < 4; i++) {
					print_val(readq(addr));
					if(write)
						writeq(ctr++, addr);
					ctr = (ctr & 0xffffffff) | (ctr << 32);
					addr += sizeof(unsigned long);
					putchar(' ');
				}
				putchar('\n');
				putchar('\r');
				break;
		}
	}
}
