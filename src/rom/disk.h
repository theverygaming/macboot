#pragma once
#include <types.h>

namespace rom {
    bool read_boot_disk(uint32_t offset, void *buf, uint32_t blocks);
}
