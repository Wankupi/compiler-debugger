DockerMode ?= true
DockerImage ?= archlinux:riscv64

UserAsm := test.s builtin.s

CC := /usr/bin/riscv64-elf-gcc
GCC := /usr/bin/riscv64-elf-g++
LD := /usr/bin/riscv64-elf-ld
OBJCOPY := /usr/bin/riscv64-elf-objcopy
OBJDUMP := /usr/bin/riscv64-elf-objdump
FILT := /usr/bin/riscv64-elf-c++filt
GDB := riscv64-elf-gdb

MARCH_STRING := rv32im_zicsr

C_FLAGS := -march=$(MARCH_STRING) -mabi=ilp32 -g -Iinclude -std=c17
CXX_FLAGS := -march=$(MARCH_STRING) -mabi=ilp32 -g -Iinclude -std=c++17 -Wno-builtin-declaration-mismatch

LINK_FLAGS := -nostdlib -m elf32lriscv

ifeq ($(DockerMode), false)
all: code.elf code.dump
else
all:
	docker run --rm -v $(shell pwd):/workspace -w /workspace $(DockerImage) make all DockerMode=false
endif

build_docker:
	docker build -t $(DockerImage) dependency

clean:
	rm -f *.o
	rm -f src/*.o
	rm -f code.elf
	rm -f code.dump
	@echo Note that this does not remove the Docker image.

code.elf: src/linker.ld src/entry.o src/libmx.o src/mini-kernel.o src/vtable.o $(UserAsm:.s=.o)
	$(LD) $(LINK_FLAGS) -T src/linker.ld src/entry.o src/libmx.o src/vtable.o $(UserAsm:.s=.o) src/mini-kernel.o -o code.elf

code.dump: code.elf
	$(OBJDUMP) -D code.elf | $(FILT) -t > code.dump


%.o: %.s
	$(CC) $(C_FLAGS) -c $< -o $@

%.o: %.cpp
	$(GCC) $(CXX_FLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(C_FLAGS) -c $< -o $@


QEMU_RUN_ARGS := -nographic -machine virt -m 256M

run: code.elf code.dump
	@qemu-system-riscv32 $(QEMU_RUN_ARGS) -bios $<

debug: code.elf code.dump
	@qemu-system-riscv32 $(QEMU_RUN_ARGS) -bios $< -s -S


gdb: code.dump
	@$(GDB) -ex 'target remote localhost:1234' -x '.gdbinit'
