#include <boot/boot.h>
#include <rom/globals.h>
#include <screen.h>
#include <stdlib.h>
#include <types.h>

extern "C" uint8_t _bss_start;
extern "C" uint8_t _bss_end;

static void clearbss() {
    for (uint8_t *addr = &_bss_start; addr < &_bss_end; addr++) {
        *addr = 0;
    }
}

static void die() {
    screen::puts("ERR   unable to boot\nERR   halt\n");
    while (true) {
        asm volatile("nop");
    }
}

extern "C" void _kcppentry() {
    clearbss();
    screen::init();
    screen::clear();
    screen::puts("OK    welcome to the funne bootloader\n");

    // resources: linux/arch/m68k/include/asm/mac_via.h, linux/arch/m68k/mac/via.c
    /*
    // uint32_t vrega = rom::VIA + 7680;
    uint32_t vregb = rom::VIA;
    uint8_t *vregb_p = (uint8_t *)vregb;
    *vregb_p &= ~(1 << 7); // enable sound
    const volatile uint32_t &pwmbuf = *(volatile const uint32_t *const)(0x0B0A);
    memset((void *)pwmbuf, 0x55, 100);
    */

    boot();

    // rom::syserror(0x69);
    die();
}
