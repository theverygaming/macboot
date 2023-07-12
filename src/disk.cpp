#include <algorithm>
#include <bumpalloc.h>
#include <disk.h>
#include <rom/disk.h>
#include <screen.h>
#include <stdlib.h>

bool disk::read(uint32_t offset, void *_buf, size_t n) {
    uint8_t *buf = (uint8_t *)_buf;
    uint8_t *buf_l = (uint8_t *)bump_malloc(512);
    if (buf_l == nullptr) {
        screen::puts("ERR   disk read out of memory\n");
        return false;
    }

    if (offset % 512 == 0) {
        size_t sectors = n / 512;
        if (!rom::read_boot_disk(offset, buf, sectors)) {
            bump_dealloc(512);
            return false;
        }
        n -= sectors * 512;
        offset += sectors * 512;
        buf = (uint8_t *)((uint32_t)buf + (sectors * 512));

        bool s = rom::read_boot_disk(offset, buf_l, 1);
        memcpy(buf, buf_l, n);
        bump_dealloc(512);
        return s;
    } else {
        while (n != 0) {
            uint32_t read = std::min(n, (size_t)512);
            uint32_t boff = offset % 512;
            if (!rom::read_boot_disk(offset - boff, buf_l, 1)) {
                bump_dealloc(512);
                return false;
            }
            memcpy(buf, &buf_l[boff], read);
            n -= read;
            offset += read;
            buf += read;
        }
        bump_dealloc(512);
        return true;
    }
}
