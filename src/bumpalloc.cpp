#include <bumpalloc.h>

uint8_t *bumpalloc_begin = nullptr;
uint8_t *bumpalloc_end = nullptr;

void *bump_malloc(size_t n) {
    if ((size_t)(bumpalloc_end - bumpalloc_begin) < n) {
        return nullptr;
    }
    uint8_t *ret = bumpalloc_begin;
    bumpalloc_begin += n;
    return ret;
}

void bump_dealloc(size_t n) {
    bumpalloc_begin -= n;
}
