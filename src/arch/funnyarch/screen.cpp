#include <screen.h>

void screen::init() {
    fb.base = (void *)0xF0000000;
    fb.width = 640;
    fb.height = 480;
    fb.pitch = 640/8;
    fb.bpp = 1;
    fb.indexed = false;
    fb.bit_be = false;
}
