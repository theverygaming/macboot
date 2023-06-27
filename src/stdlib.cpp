#include <stdlib.h>
#include <screen.h>

extern "C" void *memset(void *ptr, int value, size_t n) {
    uint8_t *p = (uint8_t *)ptr;
    while (n--)
        *p++ = (uint8_t)value;
    return ptr;
}

extern "C" void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *p = (uint8_t *)dst;
    const uint8_t *src_u = (const uint8_t *)src;
    while (n--)
        *p++ = *src_u++;
    return dst;
}

#define MALLOC_BASE 0x6000
#define MALLOC_END 0x7000
extern "C" void *malloc(size_t size) {
    static uint32_t adr = MALLOC_BASE;
    if (adr + size >= MALLOC_END) {
        screen::puts("malloc out of memory\n");
        return nullptr;
    }
    void *ret = (void *)adr;
    adr += size;
    return ret;
}
