#include <types.h>

#define MACBOOT_HAS_STDINT
#include "../../include/proto.h"

static void die() {
    while (true) {
        asm volatile("nop");
    }
}

extern "C" void *_kernel_start;
extern "C" void *_kernel_size;

void kmain();

// this is required
static volatile struct macboot_kernel_header __attribute__((section(".entry")))
header = {.id = MACBOOT_KERNEL_HEADER_ID, .load_address = (uint32_t)&_kernel_start, .size = (uint32_t)&_kernel_size, .kmain = &kmain};

// these requests are optional and just here to showcase how requests work
static volatile struct macboot_terminal_request term = {.id = MACBOOT_TERMINAL_REQUEST_ID, .response = nullptr};
static volatile struct macboot_cmdline_request cmdline = {.id = MACBOOT_CMDLINE_REQUEST_ID, .response = nullptr};

void kmain() {
    if (term.response != nullptr) {
        term.response->puts("hello from kernel\n");
        if (cmdline.response != nullptr) {
            term.response->puts("cmdline: ");
            term.response->puts(cmdline.response->cmdline);
            term.response->puts("\n");
        }
    }
    die();
}
