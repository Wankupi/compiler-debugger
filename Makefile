CC := /usr/bin/riscv64-elf-gcc
GCC := /usr/bin/riscv64-elf-g++
LD := /usr/bin/riscv32-elf-ld
OBJCOPY := /usr/bin/riscv32-elf-objcopy
OBJDUMP := /usr/bin/riscv32-elf-objdump
GDB := riscv64-elf-gdb

MARCH_STRING := rv32im_zicsr

C_FLAGS := -march=$(MARCH_STRING) -mabi=ilp32 -g -Iinclude -std=c17
CXX_FLAGS := -march=$(MARCH_STRING) -mabi=ilp32 -g -Iinclude -std=c++17 -Wno-builtin-declaration-mismatch

UserAsm := test.s builtin.s

# LINK_FLAGS := -march=rv32i -mabi=ilp32 -nostdlib
# LINK_FLAGS := -march=elf32lriscv
# src/entry.o: src/entry.s
# 	$(CC) $(C_FLAGS) -c $< -o $@

# src/libmx.cpp.o: src/libmx.cpp
# 	$(GCC) $(CXX_FLAGS) -c $< -o $@

# build/mini-kernel.cpp.o: build/mini-kernel.cpp
# 	$(GCC) $(CXX_FLAGS) -c $< -o $@

all: code.elf code.dump

clean:
	rm -f *.o
	rm -f src/*.o
	rm -f code.elf
	rm -f code.dump

code.elf: src/linker.ld src/entry.o src/libmx.o src/mini-kernel.o $(UserAsm:.s=.o)
	$(LD) $(LINK_FLAGS) -T src/linker.ld src/entry.o src/libmx.o $(UserAsm:.s=.o) src/mini-kernel.o -o code.elf

code.dump: code.elf
	$(OBJDUMP) -D code.elf > code.dump	

%.o: %.s
	$(CC) $(C_FLAGS) -c $< -o $@

%.o: %.cpp
	$(GCC) $(CXX_FLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(C_FLAGS) -c $< -o $@


QEMU_RUN_ARGS := -nographic -machine virt -m 256M

run: code.elf
	@qemu-system-riscv32 $(QEMU_RUN_ARGS) -bios $<

debug: code.elf code.dump
	@qemu-system-riscv32 $(QEMU_RUN_ARGS) -bios $< -s -S


gdb: code.dump
	@$(GDB) -ex 'target remote localhost:1234' -x '.gdbinit'
