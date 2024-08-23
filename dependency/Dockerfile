FROM archlinux:latest
RUN echo 'Server = https://mirror.sjtu.edu.cn/archlinux/$repo/os/$arch' >/etc/pacman.d/mirrorlist \
    && pacman -Sy \
    && pacman -S make riscv64-elf-gcc --noconfirm \
    && pacman -Scc --noconfirm \
    && rm -rf /var/cache/pacman/pkg/* /var/lib/pacman/sync/*
