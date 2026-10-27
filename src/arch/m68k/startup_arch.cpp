#include <stdlib.h>

extern "C" void *_bss_start;
extern "C" void *_bss_end;

static void clearbss() {
    memset((void *)&_bss_start, 0, (uint32_t)&_bss_end - (uint32_t)&_bss_start);
}


void start();

extern "C" void _cppentry() {
    clearbss();
    start();
}
