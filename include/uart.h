#pragma once

namespace uart {

const auto uart_base_addr = reinterpret_cast<volatile unsigned char *>(0x10000000ull);

const auto rbr = uart_base_addr + 0;
const auto thr = uart_base_addr + 0;
const auto ier = uart_base_addr + 1;
const auto iir = uart_base_addr + 2;
const auto fcr = uart_base_addr + 2;
const auto lcr = uart_base_addr + 3;
const auto mcr = uart_base_addr + 4;
const auto lsr = uart_base_addr + 5;
const auto msr = uart_base_addr + 6;
const auto scr = uart_base_addr + 7;

inline void uart_init() {
	*ier = 0;
	*lcr = 1 << 7;
	*thr = 0x03;
	*lcr = 0b11;
	// *fcr = 1 | (1 << 1) | (1 << 2);
	*fcr = 0; // do not enable FIFO, otherwise the first byte will be lost
	// *ier = 1 | (1 << 1); // enable read and write interrupts
	*ier = 0; // do not enable interrupts
}

inline char uart_getc() {
	while (!(*lsr & 1));
	return *rbr;
}

inline void uart_putc(char c) {
	while (!(*lsr & (1 << 5)));
	*thr = c;
}

} // namespace uart
