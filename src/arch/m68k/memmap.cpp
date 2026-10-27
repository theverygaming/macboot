#include <memmap.h>
#include <arch/rom/globals.h>
#include <algorithm>

#define MACBOOT_HAS_STDINT
#include <proto.h>

extern "C" void *_bootloader_start;
extern "C" void *_bootloader_end;
#define BL_BEGIN ((uint32_t)&_bootloader_start)
#define BL_END   ((uint32_t)&_bootloader_end)

bool memmap::iter(paddr_t *base, size_t *size, int *type, void **ctx) {
    uintptr_t i = (uintptr_t)*ctx;
    
    // NOTE: BL_BEGIN - BL_END is guranteed to be contained in range RAMBase - MemTop by startup.S
    uint32_t memstart = rom::RAMBase;
    uint32_t memtop = rom::MemTop;
    // we do not want to overwrite ROM variables or even the vector table
    memstart = std::max(memstart, (uint32_t)0x1000);

    switch (i) {
        case 0: {
            *base = BL_END;
            *size = memtop - BL_END;
            *type = MACBOOT_MEMMAP_USABLE;
            break;
        }
        case 1: {
            *base = BL_BEGIN;
            *size = BL_END - BL_BEGIN;
            *type = MACBOOT_MEMMAP_BOOTLOADER_RECLAIMABLE;
            break;
        }
        case 2: {
            *base = memstart;
            *size = BL_BEGIN - memstart;
            *type = MACBOOT_MEMMAP_USABLE;
            break;
        }
        default:
            return false;
    }

    *ctx = (void *)(i + 1);
    return true;
}
