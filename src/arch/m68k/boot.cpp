#include <boot/boot.h>
#include <bumpalloc.h>
#include <screen.h>

extern "C" void _kernelentry(uint32_t sp, uint32_t kjmp); // kernelentry.S

void enter_kernel(struct macboot_kernel_header *kheader) {
    void *newsp = bump_malloc(1024);
    if (newsp == nullptr) {
        screen::puts("ERR   out of memory\n");
        return;
    }
    uint32_t sp = (uint32_t)newsp;
    sp += 1024; // stack grows downwards
    screen::puts("OK    jumping to kernel\n");
    _kernelentry(sp, (uint32_t)kheader->kmain);
}
