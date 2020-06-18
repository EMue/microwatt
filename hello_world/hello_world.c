#include <stdint.h>
#include <stdbool.h>

#include "console.h"

#define HELLO_WORLD "Hello World\n"

int main(void)
{
	console_init();

	puts(HELLO_WORLD);

	while (1) {
		unsigned char c = getchar();
		putchar(c);
	}
}
