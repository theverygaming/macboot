#pragma once
#include <types.h>

void rom_disk_read_sector(uint32_t sector, void *buf);
void rom_serial_putc(char c);
char rom_serial_getc();
void rom_serial_puts(const char *s);
