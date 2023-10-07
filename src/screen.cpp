#include <rom/globals.h>
#include <rom/quickdraw.h>
#include <screen.h>
#include <stdlib.h>
#include <types.h>

// font source: https://github.com/dhepper/font8x8/blob/master/font8x8_basic.h (Public Domain)
static const uint8_t font[]{
    0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00, // U+0041 (A)
    0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00, // U+0042 (B)
    0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00, // U+0043 (C)
    0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00, // U+0044 (D)
    0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00, // U+0045 (E)
    0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00, // U+0046 (F)
    0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00, // U+0047 (G)
    0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00, // U+0048 (H)
    0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00, // U+0049 (I)
    0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00, // U+004A (J)
    0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00, // U+004B (K)
    0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00, // U+004C (L)
    0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00, // U+004D (M)
    0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00, // U+004E (N)
    0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00, // U+004F (O)
    0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00, // U+0050 (P)
    0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00, // U+0051 (Q)
    0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00, // U+0052 (R)
    0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00, // U+0053 (S)
    0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00, // U+0054 (T)
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00, // U+0055 (U)
    0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00, // U+0056 (V)
    0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00, // U+0057 (W)
    0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00, // U+0058 (X)
    0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00, // U+0059 (Y)
    0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00, // U+005A (Z)
    0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00, // U+0030 (0)
    0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00, // U+0031 (1)
    0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00, // U+0032 (2)
    0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00, // U+0033 (3)
    0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00, // U+0034 (4)
    0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00, // U+0035 (5)
    0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00, // U+0036 (6)
    0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00, // U+0037 (7)
    0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00, // U+0038 (8)
    0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00, // U+0039 (9)
};

static rom::GrafPort qd_gp;

struct screen::fbinfo screen::fb;

static inline void putpx(unsigned int x, unsigned int y, uint8_t state) {
    if (screen::fb.indexed) {
        if (screen::fb.bpp <= 8 || true) {
            uint32_t colors = 2;
            // pow()
            for (uint32_t i = 0; (i + 1) < screen::fb.bpp; i++) {
                colors *= 2;
            }
            uint32_t color = colors - 1;
            uint8_t mask = colors - 1;

            volatile uint8_t *ptr = (volatile uint8_t *)((uint32_t)screen::fb.base + (y * screen::fb.pitch) + (x / screen::fb.bpp));
            if (state) {
                *ptr = 0xFF;
            } else {
                *ptr = 0;
            }
        }
        return;
    }
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

static uint32_t xbase = 1;
static uint32_t ybase = 1;

static inline void drawc(unsigned int i) {
    if (xbase + 8 > screen::fb.width) {
        xbase = 1;
        ybase += 9;
    }
    if (ybase + 8 > screen::fb.height) {
        ybase = 1;
    }

    bool empty = false;
    uint32_t index = i * 8;
    if (i > sizeof(font) / 8) {
        empty = true;
    }
    uint32_t count = 0;
    for (uint32_t y = ybase; y < ybase + 8; y++) {
        uint8_t g;
        if (!empty) {
            g = font[index + (count / 8)];
        }
        for (uint32_t x = xbase; x < xbase + 8; x++) {
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
    // figured this out thanks to https://emile.sourceforge.net/
    rom::InitGraf(&qd_gp);
    struct rom::GDevice *gd = *((struct rom::GDevice **)rom::MainDevice);

    if ((gd == nullptr) || (gd->gdPMap == nullptr) || ((*gd->gdPMap)->baseAddr == nullptr) || (gd == (struct rom::GDevice *)0xAAAAAAAA) ||
        ((qd_gp.screenBits.bounds.right - qd_gp.screenBits.bounds.left) != ((*gd->gdPMap)->bounds.right - (*gd->gdPMap)->bounds.left)) ||
        ((qd_gp.screenBits.bounds.bottom - qd_gp.screenBits.bounds.top) != ((*gd->gdPMap)->bounds.bottom - (*gd->gdPMap)->bounds.top))) {
        fb.base = qd_gp.screenBits.baseAddr;
        fb.width = qd_gp.screenBits.bounds.right - qd_gp.screenBits.bounds.left;
        fb.height = qd_gp.screenBits.bounds.bottom - qd_gp.screenBits.bounds.top;
        fb.pitch = qd_gp.screenBits.rowBytes & 0x3FFF; // upper two bits are flags

        fb.bpp = 1;
        fb.indexed = false;
    } else {
        fb.base = (*gd->gdPMap)->baseAddr;
        fb.width = (*gd->gdPMap)->bounds.right - (*gd->gdPMap)->bounds.left;
        fb.height = (*gd->gdPMap)->bounds.bottom - (*gd->gdPMap)->bounds.top;
        fb.pitch = (*gd->gdPMap)->rowBytes & 0x3FFF; // upper two bits are flags

        if ((((*gd->gdPMap)->rowBytes >> 15) & 0x1) == 0) { // turns out this is a BitMap instead of a PixMap
            // lets guess it's gonna be 1bpp
            fb.bpp = 1;
            fb.indexed = false;
            return;
        }
        fb.bpp = (*gd->gdPMap)->pixelSize;

        fb.indexed = (*gd->gdPMap)->pixelType == 0;
    }
}

void screen::clear() {
    memset(fb.base, 0x0, fb.height * fb.pitch);
}

static void putc(char c) {
    if (c == ' ') {
        drawc(-1);
        return;
    }
    if (c >= '0' && c <= '9') {
        drawc(((c - '0') + 'Z' + 1) - 'A');
        return;
    }
    if (c >= 'a' && c <= 'z') {
        c -= 'a' - 'A';
    }
    if (c >= 'A' && c <= 'Z') {
        drawc(c - 'A');
        return;
    }
    if (c == '\n') {
        ybase += 9;
        xbase = 1;
        return;
    }
    drawc(-1);
}

void screen::puts(const char *str) {
    while (*str) {
        putc(*str++);
    }
}

void screen::puts(const char *str, size_t len) {
    for (size_t i = 0; i < len; i++) {
        putc(str[i]);
    }
}
