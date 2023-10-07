#include <boot/boot.h>
#include <screen.h>
#include <stdlib.h>
#include <types.h>

extern "C" void *_bss_start;
extern "C" void *_bss_end;

static void clearbss() {
    memset((void *)&_bss_start, 0, (uint32_t)&_bss_end - (uint32_t)&_bss_start);
}

static void die() {
    screen::puts("ERR   unable to boot\nERR   halt\n");
    while (true) {
        asm volatile("nop");
    }
}

void experiments();

extern "C" void _kcppentry() {
    clearbss();
    screen::init();
    screen::clear();
    screen::puts("OK    welcome to the funne bootloader\n");

    experiments();

    boot();

    // rom::syserror(0x69);
    die();
}
