#pragma once
#include <types.h>

namespace disk {
    bool read(uint32_t offset, void *_buf, size_t n);
    size_t get_boot_fat_offset();
}
