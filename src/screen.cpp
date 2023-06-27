#include <rom/globals.h>
#include <rom/quickdraw.h>
#include <screen.h>
#include <stdlib.h>
#include <types.h>

// font source: https://github.com/dhepper/font8x8/blob/master/font8x8_basic.h (Public Domain)
static const uint8_t font[]{
    0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00, // U+0041 (A)
    0x3F,
    0x66,
    0x66,
    0x3E,
    0x66,
    0x66,
    0x3F,
    0x00, // U+0042 (B)
    0x3C,
    0x66,
    0x03,
    0x03,
    0x03,
    0x66,
    0x3C,
    0x00, // U+0043 (C)
    0x1F,
    0x36,
    0x66,
    0x66,
    0x66,
    0x36,
    0x1F,
    0x00, // U+0044 (D)
    0x7F,
    0x46,
    0x16,
    0x1E,
    0x16,
    0x46,
    0x7F,
    0x00, // U+0045 (E)
    0x7F,
    0x46,
    0x16,
    0x1E,
    0x16,
    0x06,
    0x0F,
    0x00, // U+0046 (F)
    0x3C,
    0x66,
    0x03,
    0x03,
    0x73,
    0x66,
    0x7C,
    0x00, // U+0047 (G)
    0x33,
    0x33,
    0x33,
    0x3F,
    0x33,
    0x33,
    0x33,
    0x00, // U+0048 (H)
    0x1E,
    0x0C,
    0x0C,
    0x0C,
    0x0C,
    0x0C,
    0x1E,
    0x00, // U+0049 (I)
    0x78,
    0x30,
    0x30,
    0x30,
    0x33,
    0x33,
    0x1E,
    0x00, // U+004A (J)
    0x67,
    0x66,
    0x36,
    0x1E,
    0x36,
    0x66,
    0x67,
    0x00, // U+004B (K)
    0x0F,
    0x06,
    0x06,
    0x06,
    0x46,
    0x66,
    0x7F,
    0x00, // U+004C (L)
    0x63,
    0x77,
    0x7F,
    0x7F,
    0x6B,
    0x63,
    0x63,
    0x00, // U+004D (M)
    0x63,
    0x67,
    0x6F,
    0x7B,
    0x73,
    0x63,
    0x63,
    0x00, // U+004E (N)
    0x1C,
    0x36,
    0x63,
    0x63,
    0x63,
    0x36,
    0x1C,
    0x00, // U+004F (O)
    0x3F,
    0x66,
    0x66,
    0x3E,
    0x06,
    0x06,
    0x0F,
    0x00, // U+0050 (P)
    0x1E,
    0x33,
    0x33,
    0x33,
    0x3B,
    0x1E,
    0x38,
    0x00, // U+0051 (Q)
    0x3F,
    0x66,
    0x66,
    0x3E,
    0x36,
    0x66,
    0x67,
    0x00, // U+0052 (R)
    0x1E,
    0x33,
    0x07,
    0x0E,
    0x38,
    0x33,
    0x1E,
    0x00, // U+0053 (S)
    0x3F,
    0x2D,
    0x0C,
    0x0C,
    0x0C,
    0x0C,
    0x1E,
    0x00, // U+0054 (T)
    0x33,
    0x33,
    0x33,
    0x33,
    0x33,
    0x33,
    0x3F,
    0x00, // U+0055 (U)
    0x33,
    0x33,
    0x33,
    0x33,
    0x33,
    0x1E,
    0x0C,
    0x00, // U+0056 (V)
    0x63,
    0x63,
    0x63,
    0x6B,
    0x7F,
    0x77,
    0x63,
    0x00, // U+0057 (W)
    0x63,
    0x63,
    0x36,
    0x1C,
    0x1C,
    0x36,
    0x63,
    0x00, // U+0058 (X)
    0x33,
    0x33,
    0x33,
    0x1E,
    0x0C,
    0x0C,
    0x1E,
    0x00, // U+0059 (Y)
    0x7F,
    0x63,
    0x31,
    0x18,
    0x4C,
    0x66,
    0x7F,
    0x00, // U+005A (Z)
};

static rom::GrafPort qd_gp;

struct screen::fbinfo screen::fb;

static inline void putpx(unsigned int x, unsigned int y, uint8_t state) {
    switch (screen::fb.bpp) {
    case 1:
        volatile uint8_t *ptr = (volatile uint8_t *)((uint32_t)screen::fb.base + (y * screen::fb.pitch) + (x / 8));
        *ptr ^= (-state ^ *ptr) & (1 << (7 - (x % 8)));
        break;
    }
}

static inline uint8_t bitget(uint8_t num, uint8_t bit) {
    return (num & (0x1 << bit)) >> bit;
}

static int xbase = 1;
static int ybase = 1;

static inline void drawc(unsigned int i) {
    if (xbase + 8 > screen::fb.width) {
        xbase = 1;
        ybase += 9;
    }
    if (ybase + 8 > screen::fb.height) {
        ybase = 1;
    }

    bool empty = false;
    int index = i * 8;
    if (i > sizeof(font) / 8) {
        empty = true;
    }
    int count = 0;
    for (int y = ybase; y < ybase + 8; y++) {
        uint8_t g;
        if (!empty) {
            g = font[index + (count / 8)];
        }
        for (int x = xbase; x < xbase + 8; x++) {
            uint8_t offset = count % 8;
            if (empty) {
                putpx(x, y, 0);
                continue;
            }
            putpx(x, y, bitget(g, offset));
            count++;
        }
    }
    xbase += 8;
}

void screen::init() {
    rom::InitGraf(&qd_gp);
    fb.base = qd_gp.screenBits.baseAddr;
    fb.width = qd_gp.screenBits.bounds.right - qd_gp.screenBits.bounds.left;
    fb.height = qd_gp.screenBits.bounds.bottom - qd_gp.screenBits.bounds.top;
    fb.pitch = qd_gp.screenBits.rowBytes;
    fb.bpp = 1;
}

void screen::clear() {
    memset(fb.base, 0x0, fb.height * fb.pitch);
}

void screen::puts(const char *str) {
    char c;
    while (*str) {
        c = *str++;
        if (c == ' ') {
            drawc(-1);
            continue;
        }
        if (c >= 'a' && c <= 'z') {
            c -= 'a' - 'A';
        }
        if (c >= 'A' && c <= 'Z') {
            drawc(c - 'A');
            continue;
        }
        if (c == '\n') {
            ybase += 9;
            xbase = 1;
            continue;
        }
        drawc(-1);
    }
}
