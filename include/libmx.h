#pragma once
using size_t = unsigned long;

extern "C" int putchar(int ch);
extern "C" int puts(const char *str);
extern "C" int getchar();
extern "C" void *memset(void *addr, int c, size_t n);
extern "C" void *memcpy(void *str1, const void *str2, size_t n);
extern "C" int memcmp(const void *str1, const void *str2, size_t n);
extern "C" void *my_memmove(void *dest, const void *src, size_t n);
extern "C" void *malloc(size_t size);
extern "C" void *calloc(size_t nmemb, size_t size);
extern "C" void free(void *ptr);
extern "C" void *realloc(void *ptr, size_t size);
extern "C" char *strcpy(char *dest, const char *src);
extern "C" size_t strlen(const char *str);
extern "C" char *strcat(char *dest, const char *src);
extern "C" int strcmp(const char *str1, const char *str2);
extern "C" void print_int(int a);

extern "C" void scanf(const char *format, void *output);
extern "C" void printf(const char *format, ...);
extern "C" void sscanf();
extern "C" void sprintf();

void print_string(const char *str);
void print_hex_full(unsigned int a);
