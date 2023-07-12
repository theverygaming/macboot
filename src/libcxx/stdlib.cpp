#include <screen.h>
#include <stdlib.h>

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

extern "C" char *strncpy(char *destination, const char *source, size_t num) {
    char *destination_s = destination;
    bool copy = true;
    char c;
    for (size_t i = 0; i < num; i++) {
        if (copy) {
            c = *source++;
            if (c == '\0') {
                copy = false;
            }
        }
        *destination = c;
        destination++;
    }
    return destination_s;
}
