#pragma once
#include <types.h>

namespace rom {
    // offset must be aligned to block size (512)
    bool read_boot_disk(uint32_t offset, void *buf, uint32_t blocks);
}
