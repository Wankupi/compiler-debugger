#include "libmx.h"
#include "uart.h"
#include "vtable.h"

extern "C" int main();
extern "C" void _trap_entry();

void exception_init() {
	asm volatile("csrw mtvec, %0" ::"r"(_trap_entry));
}

extern unsigned int trap_entry_regs[32];

extern "C" void trap_handler() {
	int mcause = 0;
	asm volatile("csrr %0, mcause" : "=r"(mcause));
	if (mcause == 8) {
		print_string("\033[32m%");
		print_int(trap_entry_regs[10]);
		print_string("\033[0m\n");
	}
	else {
		int mepc = 0;
		asm volatile("csrr %0, mepc" : "=r"(mepc));
		print_string("\033[31m<");
		print_int(mcause);
		print_string(">\033[0m ");
		print_string("trap from ");
		print_hex_full(mepc);
		print_string("\n");
	}
	shutdown();
}

void user_start() {
	int r = main();
	// ecall to exit
	// set a0 = r
	asm volatile("mv a0, %0; ecall" ::"r"(r));
}


PageTableManager ptm;

extern "C" void stext();
extern "C" void etext();
extern "C" void srodata();
extern "C" void erodata();
extern "C" void sdata();
extern "C" void edata();
extern "C" void sbss();
extern "C" void ebss();

void init_pagetable() {
	if (ptm.root_page_table == nullptr)
		ptm = PageTableManager();
	auto map = [](auto start, auto end, auto flags) {
		uint32_t size = (uint32_t) (end) - (uint32_t) (start);
		size = (size + 0xfff) & ~0xfff;
		ptm.map((uint32_t) start, size, flags);
	};
	map(stext, etext, PTE_R | PTE_X | PTE_U);
	map(srodata, erodata, PTE_R | PTE_U);
	map(sdata, edata, PTE_R | PTE_W | PTE_U);
	map(sbss, ebss, PTE_R | PTE_W | PTE_U);
	map(MALLOC_START_ADDR, MALLOC_END_ADDR, PTE_R | PTE_W | PTE_U);
	ptm.map((uint32_t) uart::uart_base_addr, 0x1000, PTE_R | PTE_W | PTE_U);
	ptm.map(0x100000, 0x1000, PTE_W | PTE_U);
	ptm.set_satp();
}

void init_pmp() {
	int pmp0cfg = 0b11 << 3 | 0b111;
	int pmpaddr0 = 0xffffffff;
	asm volatile("csrw pmpaddr0, %0" ::"r"(pmpaddr0));
	asm volatile("csrw pmpcfg0, %0" ::"r"(pmp0cfg));
}

void set_mcsr() {
	int mie = 3;
	asm volatile("csrw mie, %0" ::"r"(mie));
	unsigned int mstatus = 0;
	asm volatile("csrr %0, mstatus" : "=r"(mstatus));
	mstatus |= 1 << 3; // enable mie
	// set mpp to user mode
	mstatus &= ~(3 << 11);
	// write back
	asm volatile("csrw mstatus, %0" ::"r"(mstatus));
	// set mepc to user_start
	asm volatile("csrw mepc, %0" ::"r"(user_start));
}

extern "C" void mini_kernel_main() {
	// init uart
	uart::uart_init();
	// init trap handler
	exception_init();
	// init page table
	init_pagetable();
	init_pmp();
	// call main
	set_mcsr();
}
