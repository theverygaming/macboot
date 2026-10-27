#pragma once
#include <types.h>

namespace screen {
    void init();
    void clear();
    void puts(const char *str);
    void puts(const char *str, size_t len);
    extern struct fbinfo {
        void *base;
        size_t width;
        size_t height;
        size_t pitch;
        unsigned short bpp;
        bool indexed;
        bool bit_be; // if true the rightmost pixel is LSB, not MSB
    } fb;
}
