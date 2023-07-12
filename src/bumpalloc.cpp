#include <bumpalloc.h>

extern "C" void *_malloc_begin;
extern "C" void *_bootloader_end;

static uint8_t *begin = (uint8_t *)&_malloc_begin;
static uint8_t *end = (uint8_t *)&_bootloader_end;

void *bump_malloc(size_t n) {
    if ((end - begin) < n) {
        return nullptr;
    }
    uint8_t *ret = begin;
    begin += n;
    return ret;
}

void bump_dealloc(size_t n) {
    begin -= n;
}
