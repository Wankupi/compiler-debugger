#include "libmx.h"
#include "uart.h"
#include <cstdarg>

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
	// note that EOF is not supported now
	return (unsigned char) uart::uart_getc();
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
extern "C" void *memmove(void *dest, const void *src, size_t n) {
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


void *heap_top = reinterpret_cast<void *>(MALLOC_START_ADDR);

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

extern "C" void print_int(int a) {
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

const int EOF = -1;

bool isspace(char c) {
	return (c == ' ' ||  // 空格
			c == '\n' || // 换行符
			c == '\t' || // 水平制表符
			c == '\r' || // 回车符
			c == '\v' || // 垂直制表符
			c == '\f');  // 换页符
}

extern "C" void scanf(const char *format, void *output) {
	int c;
	if (format[0] == '%' && format[2] == '\0') {
		switch (format[1]) {
			case 'd': {
				int num = 0;
				int sign = 1;
				// 处理正负号
				c = getchar();
				if (c == '-') {
					sign = -1;
					c = getchar();
				}
				// 读取数字
				while (c >= '0' && c <= '9') {
					num = num * 10 + (c - '0');
					c = getchar();
				}
				*(int *) output = num * sign;
				break;
			}
			case 'c': {
				// 读取单个字符
				c = getchar();
				*(char *) output = c;
				break;
			}
			case 's': {
				// 读取字符串，直到遇到空白字符或EOF
				char *str = (char *) output;
				c = getchar();
				while (!isspace(c) && c != EOF) {
					*str++ = c;
					c = getchar();
				}
				*str = '\0'; // 字符串结束符
				break;
			}
			default:
				// 如果需要支持其他格式，请提issue
				break;
		}
	}
}

// 辅助函数：输出一个字符
void print_char(char c) {
	putchar(c);
}

// 辅助函数：输出一个字符串
void print_string(const char *str) {
	while (*str) {
		putchar(*str++);
	}
}

void print_hex(unsigned int a) {
	char buf[16];
	int i = 0;
	while (a) {
		int t = a % 16;
		if (t < 10) {
			buf[i++] = t + '0';
		}
		else {
			buf[i++] = t - 10 + 'a';
		}
		a /= 16;
	}
	if (i == 0) {
		putchar('0');
	}
	while (i--) {
		putchar(buf[i]);
	}
}

void print_hex_full(unsigned int a) {
	// always 8 digit
	putchar('0');
	putchar('x');
	for (int i = 28; i >= 0; i -= 4) {
		int t = (a >> i) & 0xf;
		if (t < 10) {
			putchar(t + '0');
		}
		else {
			putchar(t - 10 + 'a');
		}
	}
}

// 实现 printf 函数
void printf(const char *format, ...) {
	va_list args;
	va_start(args, format);

	while (*format) {
		if (*format == '%') {
			format++;
			switch (*format) {
				case 'd': {
					int value = va_arg(args, int);
					print_int(value);
					break;
				}
				case 'c': {
					char value = static_cast<char>(va_arg(args, int)); // char 参数提升为 int
					print_char(value);
					break;
				}
				case 's': {
					const char *value = va_arg(args, const char *);
					print_string(value);
					break;
				}
				case 'x': {
					unsigned int value = va_arg(args, unsigned int);
					print_hex(value);
					break;
				}
				default:
					// 处理不支持的格式符，输出原样
					putchar('%');
					putchar(*format);
					break;
			}
		}
		else {
			putchar(*format);
		}
		format++;
	}
	va_end(args);
}

extern "C" void sscanf() {
	// not implemented
}
extern "C" void sprintf() {
	// not implemented
}

extern "C" void shutdown() {
	*reinterpret_cast<volatile unsigned int *>(0x100000) = 0x5555;
}
