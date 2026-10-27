#include <memmap.h>
#include <arch/memmap.h>
#include <arch/config.h>

#define LITTLEBOOT_HAS_STDINT
#include <proto.h>

bool memmap::iter(paddr_t *base, size_t *size, int *type, void **ctx) {
    uintptr_t i = (uintptr_t)*ctx;

    switch (i) {
        case 0: {
            *base = mem_reclaimable1_start;
            *size = mem_reclaimable1_size;
            *type = LITTLEBOOT_MEMMAP_BOOTLOADER_RECLAIMABLE;
            break;
        }
        case 1: {
            *base = mem_reclaimable2_start;
            *size = mem_reclaimable2_size;
            *type = LITTLEBOOT_MEMMAP_BOOTLOADER_RECLAIMABLE;
            break;
        }
        case 2: {
            *base = mem_usable_start;
            *size = mem_usable_size;
            *type = LITTLEBOOT_MEMMAP_USABLE;
            break;
        }
        default:
            return false;
    }

    *ctx = (void *)(i + 1);
    return true;
}

uint32_t mem_reclaimable1_start;
uint32_t mem_reclaimable1_size;
uint32_t mem_reclaimable2_start;
uint32_t mem_reclaimable2_size;
uint32_t mem_bumpalloc_start;
uint32_t mem_bumpalloc_size;
uint32_t mem_usable_start;
uint32_t mem_usable_size;

extern "C" uint8_t _bootloader_start;
extern "C" uint8_t _bootloader_end;

bool funnyarch_mem_init(uint32_t membase, uint32_t membytes) {
    // bootloader code & bumpalloc
    size_t bootloader_size = &_bootloader_end - &_bootloader_start;
    mem_reclaimable1_start = membase;
    mem_reclaimable1_size = bootloader_size + BOOTLOADER_BUMPALLOC_BYTES;
    // bootloader stack
    mem_reclaimable2_start = ((membase + membytes) & ~0x3) - BOOTLOADER_STACK_SIZE;
    mem_reclaimable2_size = BOOTLOADER_STACK_SIZE;
    // bootloader bumpalloc
    mem_bumpalloc_start = membase + bootloader_size;
    mem_bumpalloc_size = BOOTLOADER_BUMPALLOC_BYTES;
    // remaining memory
    mem_usable_start = (mem_reclaimable1_start + mem_reclaimable1_size);
    mem_usable_size = mem_reclaimable2_start - (mem_reclaimable1_start + mem_reclaimable1_size);

    // make sure things actually fit :)
    if ((mem_reclaimable1_start + mem_reclaimable1_size) > mem_reclaimable2_start) {
        return false;
    }

    return true;
}
