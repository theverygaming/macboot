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
