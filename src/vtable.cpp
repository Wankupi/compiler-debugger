#include "vtable.h"
#include "libmx.h"

// PPN（物理页号）相关
constexpr uint32_t PPN_MASK_0 = 0xFFFFF000; // 12 bit offset
constexpr uint32_t PPN_MASK_1 = 0xFFC00000; // 22 bit offset

constexpr int POOL_SIZE = 256; // 1MB


alignas(4096) PageTable pool[POOL_SIZE];

PageTable *allocate_page_table() {
	static PageTable *pool_ptr = pool;
	if (pool_ptr == pool + POOL_SIZE) {
		print_string("Allocate PageTable failed\n");
		shutdown();
	}
	memset(pool_ptr, 0, sizeof(PageTable));
	// print_string("allocate page table at ");
	// print_hex_full(reinterpret_cast<uint32_t>(pool_ptr));
	return pool_ptr++;
}

// 获取虚拟地址的 PTE 索引
constexpr uint32_t getIndex(uint32_t virt_addr, int level) {
	uint32_t index_shift = 12 + level * 10;
	return (virt_addr >> index_shift) & 0x3FF;
}

uint32_t paddr2pte(uint32_t paddr, uint32_t flags) {
	return (paddr >> 12) << 10 | flags | PTE_V;
}
PageTable *pte2pt(uint32_t pte) {
	return reinterpret_cast<PageTable *>((pte >> 10) << 12);
}

// sv32 normal page 12bit, use level 1 and 0
void map_page12(PageTable *rt_table, uint32_t addr, uint32_t flags) {
	uint32_t vpn1 = getIndex(addr, 1);
	uint32_t vpn0 = getIndex(addr, 0);
	uint32_t &pte1 = rt_table->entries[vpn1];
	if (!(pte1 & PTE_V)) {
		uint32_t pte0 = reinterpret_cast<uint32_t>(allocate_page_table());
		pte1 = paddr2pte(pte0, PTE_V);
	}
	PageTable *level1 = pte2pt(pte1);
	level1->entries[vpn0] = paddr2pte(addr, flags | PTE_V);
}

// sv32 big page 22bit, use level 1 only
void map_page22(PageTable *rt_table, uint32_t addr, uint32_t flags) {
	uint32_t vpn1 = getIndex(addr, 1);
	uint32_t &pte1 = rt_table->entries[vpn1];
	pte1 = paddr2pte(addr, flags | PTE_V);
}

void PageTableManager::map(uint32_t addr, uint32_t size, uint32_t flags) {
	if (addr & 0xFFF) {
		print_string("Unaligned address\n");
		shutdown();
	}
	while (size && (addr & 0x3FFFFF)) {
		map_page12(root_page_table, addr, flags);
		addr += 0x1000;
		size -= 0x1000;
	}
	while (size >= (1 << 22)) {
		map_page22(root_page_table, addr, flags);
		addr += 1 << 22;
		size -= 1 << 22;
	}
	while (size) {
		map_page12(root_page_table, addr, flags);
		addr += 0x1000;
		size -= 0x1000;
	}
}

PageTableManager::PageTableManager() {
	root_page_table = allocate_page_table();
}
