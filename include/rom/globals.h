#pragma once
#include <types.h>

namespace rom {
    static volatile const uint32_t &RAMBase = *(volatile const uint32_t *const)(0x02b2);
    static volatile const uint32_t &MemTop = *(volatile const uint32_t *const)(0x0108);
    static volatile const uint32_t &VIA = *(volatile const uint32_t *const)(0x01D4);
    static volatile const uint16_t &BootDrive = *(volatile const uint16_t *const)(0x0210);
    static volatile const uint16_t &BtDskRfn = *(volatile const uint16_t *const)(0x0B34);
}
