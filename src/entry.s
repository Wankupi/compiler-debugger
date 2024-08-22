    .extern  main
    .extern  mini_kernel_main
    .extern  trap_handler

    .section .text._start
    .global  _start
_start:
    la       sp, boot_stack_top
    tail     mini_kernel_main


    .macro   save_reg base, n
    sw       x\n, \n*8(\base)
    .endm


    .section .text._trap_entry
    .align   2
    .global  _trap_entry
_trap_entry:
    csrw     mscratch, a0
    la       a0, trap_entry_regs

    save_reg a0, 0
    save_reg a0, 1
    save_reg a0, 2
    save_reg a0, 3
    save_reg a0, 4
    save_reg a0, 5
    save_reg a0, 6
    save_reg a0, 7
    save_reg a0, 8
    save_reg a0, 9
# save_reg a0, 10
    save_reg a0, 11
    save_reg a0, 12
    save_reg a0, 13
    save_reg a0, 14
    save_reg a0, 15
    save_reg a0, 16
    save_reg a0, 17
    save_reg a0, 18
    save_reg a0, 19
    save_reg a0, 20
    save_reg a0, 21
    save_reg a0, 22
    save_reg a0, 23
    save_reg a0, 24
    save_reg a0, 25
    save_reg a0, 26
    save_reg a0, 27
    save_reg a0, 28
    save_reg a0, 29
    save_reg a0, 30
    save_reg a0, 31
# store a0
    mv       a1, a0
    csrr     a0, mscratch
    save_reg a1, 10
    tail     trap_handler



    .section .bss
    .align   12
trap_entry_regs: # 分配空间用于保存寄存器的数组
    .word    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    .word    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0



    .section .bss.stack.boot_stack_lower_bound
    .global  boot_stack_lower_bound
boot_stack_lower_bound:
    .space   4096 * 10
    .section .bss.stack.boot_stack_top
    .global  boot_stack_top
boot_stack_top:
