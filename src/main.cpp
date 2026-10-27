#include <boot/boot.h>
#include <screen.h>
#include <stdlib.h>
#include <types.h>
#include <bumpalloc.h>

static void die() {
    screen::puts("ERR   unable to boot\nERR   halt\n");
    while (true) {
        asm volatile("nop");
    }
}

void experiments();

void start() {
    bumpalloc_init();
    screen::init();
    screen::clear();
    screen::puts("OK    welcome to the funne bootloader\n");

    // experiments();

    boot();

    die();
}
