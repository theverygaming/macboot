#include <boot/boot.h>
#include <arch/config.h>
#include <screen.h>
#include <bumpalloc.h>

extern "C" void _kernelentry(uint32_t sp, uint32_t kjmp); // kernelentry.S

void enter_kernel(struct littleboot_kernel_header *kheader) {
    void *newsp = bump_malloc(KERNEL_STACK_SIZE);
    if (newsp == nullptr) {
        screen::puts("ERR   out of memory\n");
        return;
    }
    uint32_t sp = (uint32_t)newsp;
    sp += KERNEL_STACK_SIZE; // stack grows downwards
    screen::puts("OK    jumping to kernel\n");
    _kernelentry(sp, (uint32_t)kheader->kmain);
}
