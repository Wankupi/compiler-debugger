#pragma once
#include "libmx.h"

using uint32_t = unsigned int;
// 页表相关定义
constexpr uint32_t PAGE_SIZE = 4096;                 // 页大小 4KB
constexpr uint32_t PTE_SIZE = 4;                     // 页表项大小
constexpr uint32_t PTE_COUNT = PAGE_SIZE / PTE_SIZE; // 页表项数量

// 标志位定义
constexpr uint32_t PTE_V = 0x001; // 有效位
constexpr uint32_t PTE_R = 0x002; // 可读
constexpr uint32_t PTE_W = 0x004; // 可写
constexpr uint32_t PTE_X = 0x008; // 可执行
constexpr uint32_t PTE_U = 0x010; // 用户态

// 页表结构体
struct PageTable {
	uint32_t entries[PTE_COUNT]; // 页表项数组
};

struct PageTableManager {
	PageTable *root_page_table; // 根页表
public:
	PageTableManager();
	void map(uint32_t addr, uint32_t size, uint32_t flags);
	void set_satp() {
		auto satp = (1u << 31) | (reinterpret_cast<uint32_t>(root_page_table) >> 12);
		asm volatile("csrw satp, %0" ::"r"(satp));
		asm volatile("sfence.vma");
	}
};
