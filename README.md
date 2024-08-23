# 编译器辅助调试器

QEMU and GDB Debug Bridge for Compilers

## 用法

1. 将编译得到的汇编文件拷贝到当前目录下。如 builtin.s 和 test.s。
2. 确保 Makefile 中 UserAsm 的值为上一步中的文件名，多个文件空格分隔即可。
3. - 运行 make run 编译运行
   - 运行 make debug 编译调试，在另一个终端中运行 make gdb

在调试过程中，你可以打开 code.dump 来查看反汇编代码。


## 环境配置

环境的配置分为三部分。

- 安装 riscv32 编译器。在这个项目里实际使用的是 riscv64 gcc。
  + 最简单的配置方法是使用 Dockerfile。如果你已经安装了 docker，只需要在此目录运行 `make build_docker`。然后修改 Makefile 中 `DockerMode` 为 `true`。
  + 在 Archlinux 上使用 pacman 安装。参考 Dockerfile。
  + 自行编译 gcc。[参考链接](https://github.com/riscv-collab/riscv-gnu-toolchain)。
- 安装 qemu-system-riscv32。
  + 如果你是 archlinux，可以直接安装包 `qemu-system-riscv`。
  + 如果你是 ubuntu, 可以考虑安装 `qemu-system-misc`。
  + 如果你不想在本地安装巨大的软件包，可以从[这里下载](https://jbox.sjtu.edu.cn/l/c1bREL)包含qemu-system-riscv32及依赖库的最小环境。你可以考虑使用docker，仿照目标all进行调用；或者更改命令为
  `cp code.elf /path/here && sudo chroot /path/to/it /usr/bin/qemu-system-riscv32 ...`。
- 安装 gdb。
  + 如果你是 archlinux，安装 `riscv64-elf-gdb`。
  + 如果你是 ubuntu, 安装 `gdb-multiarch`，并修改 Makefile。

