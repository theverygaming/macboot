#include <types.h>
#include <arch/memmap.h>
#include <arch/rom.h>

void start();

extern "C" void _cppentry(uint32_t membase, uint32_t membytes) {
    // TODO: clear .bss _when there is one to clear, currently the .bss is included in the image_
    rom_serial_puts("entered bootloader!\n");
    if (!funnyarch_mem_init(membase, membytes)) {
        rom_serial_puts("error: funnyarch_mem_init failed\n");
        return;
    }
    start();
}
