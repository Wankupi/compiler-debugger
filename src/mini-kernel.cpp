#include "libmx.h"
#include "uart.h"

extern "C" int main();

void shutdown() {
	*reinterpret_cast<volatile unsigned int *>(0x100000) = 0x5555;
}

extern "C" void mini_kernel_main() {
	uart::uart_init();
	int ret = main();
	puts("\x1b[32m%");
	printInt(ret);
	puts("\x1b[0m");
	shutdown();
}
