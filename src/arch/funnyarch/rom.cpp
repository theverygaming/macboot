#include <arch/rom.h>

extern "C" uint32_t romcall(uint32_t no, ...);

void rom_disk_read_sector(uint32_t sector, void *buf) {
    romcall(0, sector, (uint32_t)buf);
}

void rom_serial_putc(char c) {
    romcall(1, (uint32_t)c);
}

char rom_serial_getc() {
    return romcall(2);
}

void rom_serial_puts(const char *s) {
    romcall(3, (uint32_t)s);
}
