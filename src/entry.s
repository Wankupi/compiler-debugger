    .extern  main
    .extern  mini_kernel_main

    .section .text._start
    .globl   _start
_start:
    la       sp, boot_stack_top
    tail     mini_kernel_main

    .section .bss.stack.boot_stack_lower_bound
    .globl   boot_stack_lower_bound
boot_stack_lower_bound:
    .space   4096 * 10
    .section .bss.stack.boot_stack_top
    .globl   boot_stack_top
boot_stack_top:
