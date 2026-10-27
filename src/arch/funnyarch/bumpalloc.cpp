#include <types.h>
#include <bumpalloc.h>
#include <arch/config.h>
#include <arch/memmap.h>

void bumpalloc_init() {
    bumpalloc_begin = (uint8_t *)mem_bumpalloc_start;
    bumpalloc_begin = (uint8_t *)((((uintptr_t)bumpalloc_begin) + 3) & ~0x3); // align
    bumpalloc_end = (uint8_t *)(mem_bumpalloc_start + mem_bumpalloc_size);
}
