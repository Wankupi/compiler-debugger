set disassemble-next-line on
set arch riscv:rv32
add-symbol-file code.elf -readnow

b _trap_entry
