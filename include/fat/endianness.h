#pragma once
#include <types.h>

template <typename T> T swap_le(T x) {
    union s {
        T x;
        uint8_t p[sizeof(T)];
    } in, out;
    in.x = x;

    for (size_t i = 0; i < sizeof(T); i++) {
        out.p[i] = in.p[(sizeof(T) - i) - 1];
    }

    return out.x;
}
