#include <disk.h>
#include <arch/rom.h>
#include <bumpalloc.h>
#include <stdlib.h>
#include <screen.h>
#include <algorithm>

bool disk::read(uint32_t offset, void *_buf, size_t n) {
    uint8_t *buf = (uint8_t *)_buf;

    if (offset % 512 == 0 && n % 512 == 0) {
        size_t sectors = n / 512;
        while (sectors > 0) {
            rom_disk_read_sector(offset / 512, buf);
            buf += 512;
            sectors--;
            offset += 512;
        }
        return true;
    }

    uint8_t *buf_l = (uint8_t *)bump_malloc(512);
    if (buf_l == nullptr) {
        screen::puts("ERR   disk read out of memory\n");
        return false;
    }
    while (n != 0) {
        uint32_t boff = offset % 512;
        uint32_t read = std::min(n, (size_t)512 - boff);
        rom_disk_read_sector((offset - boff) / 512, buf_l);
        memcpy(buf, &buf_l[boff], read);
        n -= read;
        offset += read;
        buf += read;
    }
    bump_dealloc(512);
    return true;
}

extern "C" uint8_t _bootloader_start;
extern "C" uint8_t _bootloader_end;

size_t disk::get_boot_fat_offset() {
    return &_bootloader_end - &_bootloader_start;
}
