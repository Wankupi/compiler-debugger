#include "libmx.h"
#include "uart.h"

extern "C" int putchar(int ch) {
	uart::uart_putc(ch);
	return ch;
}

extern "C" int puts(const char *str) {
	int count = 1;
	while (*str) {
		uart::uart_putc(*str++);
		count++;
	}
	return count;
}

extern "C" int getchar() {
	return uart::uart_getc();
}

extern "C" void *memset(void *addr, int c, size_t n) {
	auto start = reinterpret_cast<char *>(addr);
	auto ed = start + n;
	while (start != ed)
		*start++ = c;
	return addr;
}
extern "C" void *memcpy(void *str1, const void *str2, size_t n) {
	auto ret = str1;
	while (n--)
		*reinterpret_cast<char *&>(str1)++ = *reinterpret_cast<char const *&>(str2)++;
	return ret;
}
extern "C" int memcmp(const void *str1, const void *str2, size_t n) {
	while (n--) {
		auto a = *reinterpret_cast<const char *&>(str1)++;
		auto b = *reinterpret_cast<const char *&>(str2)++;
		if (a < b)
			return -1;
		else if (a > b)
			return 1;
	}
	return 0;
}
extern "C" void *my_memmove(void *dest, const void *src, size_t n) {
	unsigned char *d = static_cast<unsigned char *>(dest);
	const unsigned char *s = static_cast<const unsigned char *>(src);

	if (src > dest) {
		// left -> right
		while (n--)
			*d++ = *s++;
	}
	else if (src < dest) {
		d += n;
		s += n;
		while (n--)
			*--d = *--s;
	}
	return dest;
}


void *heap_top = reinterpret_cast<void *>(0x81000000);

constexpr size_t ALIGN = 64;

static void set_size_for_malloced_mem(void *ptr, size_t size) {
	auto p = reinterpret_cast<char *>(ptr) - ALIGN;
	*reinterpret_cast<size_t *>(p) = size;
}

static size_t get_size_from_malloced_mem(void const *ptr) {
	if (!ptr) return 0;
	auto p = reinterpret_cast<char const *>(ptr) - ALIGN;
	return *reinterpret_cast<size_t const *>(p);
}

extern "C" void *malloc(size_t size) {
	size_t aligned_size = (size + (ALIGN - 1)) & ~(ALIGN - 1);
	aligned_size += ALIGN;
	auto ret = reinterpret_cast<char *>(heap_top);
	heap_top = ret + aligned_size;
	ret += ALIGN;
	set_size_for_malloced_mem(ret, size);
	return ret;
}

extern "C" void *calloc(size_t nmemb, size_t size) {
	size_t total_size = nmemb * size;
	void *ptr = malloc(total_size);
	memset(ptr, 0, total_size);
	return ptr;
}

extern "C" void free(void *ptr) {
	// do nothing
}

extern "C" void *realloc(void *ptr, size_t size) {
	auto sz_old = get_size_from_malloced_mem(ptr);
	if (sz_old >= size) {
		set_size_for_malloced_mem(ptr, size);
		return ptr;
	}
	auto ptr2 = malloc(size);
	memcpy(ptr2, ptr, sz_old);
	free(ptr);
	return ptr2;
}

extern "C" char *strcpy(char *dest, const char *src) {
	char *ret = dest;
	while ((*dest++ = *src++));
	return ret;
}

extern "C" size_t strlen(const char *str) {
	size_t count = 0;
	while (*str++)
		count++;
	return count;
}

extern "C" char *strcat(char *dest, const char *src) {
	char *ret = dest;
	while (*dest) dest++;
	while ((*dest++ = *src++));
	return ret;
}

extern "C" int strcmp(const char *str1, const char *str2) {
	while (*str1 && *str2) {
		if (*str1 < *str2)
			return -1;
		else if (*str1 > *str2)
			return 1;
		str1++;
		str2++;
	}
	if (*str1 < *str2)
		return -1;
	else if (*str1 > *str2)
		return 1;
	return 0;
}

extern "C" void printInt(int a) {
	if (a == 0) {
		putchar('0');
		return;
	}
	if (a < 0) {
		putchar('-');
		a = -a;
	}
	char buf[16];
	int i = 0;
	while (a) {
		buf[i++] = a % 10 + '0';
		a /= 10;
	}
	while (i--)
		putchar(buf[i]);
}


extern "C" void scanf() {}
extern "C" void printf() {}
extern "C" void sscanf() {}
extern "C" void sprintf() {}
