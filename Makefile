DockerMode ?= true
DockerImage ?= archlinux:riscv64

UserAsm := test.s builtin.s

CC := riscv64-elf-gcc
GCC := riscv64-elf-g++
LD := riscv64-elf-ld
OBJCOPY := riscv64-elf-objcopy
OBJDUMP := riscv64-elf-objdump
FILT := riscv64-elf-c++filt
GDB := riscv64-elf-gdb

MARCH_STRING := rv32im_zicsr

C_FLAGS := -march=$(MARCH_STRING) -mabi=ilp32 -g -Iinclude -std=c17
CXX_FLAGS := -march=$(MARCH_STRING) -mabi=ilp32 -g -Iinclude -std=c++17 -Wno-builtin-declaration-mismatch

LINK_FLAGS := -nostdlib -m elf32lriscv

dep_of_all := code.elf code.dump
ifeq ($(DockerMode), false)
all: $(dep_of_all)
else
need_run := $(shell make -q $(dep_of_all) DockerMode=false || echo true)
ifeq ($(need_run), true)
all:
	docker run --rm -v .:/workspace -w /workspace $(DockerImage) make all DockerMode=false
else
all:
endif
endif

build_docker:
	docker build -t $(DockerImage) dependency

# the compile rules only work when DockerMode is false
ifeq ($(DockerMode), false)

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

endif


QEMU_RUN_ARGS := -nographic -machine virt -m 256M

run: all
	@qemu-system-riscv32 $(QEMU_RUN_ARGS) -bios code.elf

debug: all
	@qemu-system-riscv32 $(QEMU_RUN_ARGS) -bios code.elf -s -S

gdb: all
	@$(GDB) -ex 'target remote localhost:1234' -x '.gdbinit'

clean:
	rm -f *.o
	rm -f src/*.o
	rm -f code.elf
	rm -f code.dump
	@echo Note that this does not remove the Docker image.
