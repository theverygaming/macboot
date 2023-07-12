#pragma once
#include <types.h>

namespace rom {
    static const uint32_t *const RAMBase = (const uint32_t *const)0x02b2;
    static const uint32_t *const MemTop = (const uint32_t *const)0x0108;
    static const uint32_t *const VIA = (const uint32_t *const)0x01D4;
    static const uint16_t *const BootDrive = (const uint16_t *const)0x0210;
    static const uint16_t *const BtDskRfn = (const uint16_t *const)0x0B34;
}
