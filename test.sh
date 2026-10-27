#!/usr/bin/env bash
set -e

# build bootloader
make clean && make

# build kernel
cd example
make clean && make
cd ..

# build filesystem
touch fs.img
truncate --size 1M fs.img
mkfs.fat -F12 fs.img -v
mmd -i fs.img ::boot
mcopy -i fs.img example/kernel.bin ::/boot/kernel.bin
mcopy -i fs.img example/boot.cfg ::/
mdir -i fs.img ::/
mdir -i fs.img ::/boot

cat startup.bin fs.img > disk.img
