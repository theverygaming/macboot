#pragma once
#include <types.h>

namespace screen {
    void init();
    void clear();
    void puts(const char *str);
    void puts(const char *str, size_t len);
    extern struct fbinfo {
        void *base;
        uint32_t width;
        uint32_t height;
        uint32_t pitch;
        uint32_t bpp;
    } fb;
}
