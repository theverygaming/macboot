#include <bumpalloc.h>

extern "C" void *_malloc_begin;
extern "C" void *_bootloader_end;

void bumpalloc_init() {
    bumpalloc_begin = (uint8_t *)&_malloc_begin;
    bumpalloc_end = (uint8_t *)&_bootloader_end;
}
