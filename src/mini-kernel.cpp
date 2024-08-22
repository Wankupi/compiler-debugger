#include "libmx.h"
#include "uart.h"

extern "C" int main();
extern "C" void _trap_entry();

void shutdown() {
	*reinterpret_cast<volatile unsigned int *>(0x100000) = 0x5555;
}

void exception_init() {
	asm volatile("csrw mtvec, %0" ::"r"(_trap_entry));
}

extern "C" void trap_handler() {
	print_string("trapped from addr = ");
	void *mpc = nullptr;
	asm volatile("csrr %0, mepc" : "=r"(mpc));
	print_hex_full(reinterpret_cast<unsigned int>(mpc));
	putchar('\n');
	shutdown();
}

extern "C" void mini_kernel_main() {
	// init uart
	uart::uart_init();
	// init trap handler
	exception_init();
	// call main
	int ret = main();
	print_string("\x1b[32m%");
	print_int(ret);
	print_string("\x1b[0m\n");
	shutdown();
}
