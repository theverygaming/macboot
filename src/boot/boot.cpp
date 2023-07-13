#include <algorithm>
#include <boot/boot.h>
#include <boot/config.h>
#include <bumpalloc.h>
#include <disk.h>
#include <fat/fat.h>
#include <rom/globals.h>
#include <screen.h>
#include <stdlib.h>
#include <types.h>

#define MACBOOT_HAS_STDINT
#include <proto.h>

extern "C" void *_bootloader_start;
extern "C" void *_bootloader_end;
#define BL_BEGIN ((uint32_t)&_bootloader_start)
#define BL_END   ((uint32_t)&_bootloader_end)

static struct macboot_memmap_response *gen_memmap() {
    // NOTE: BL_BEGIN - BL_END is guranteed to be contained in range RAMBase - MemTop by startup.S
    uint32_t memstart = rom::RAMBase;
    uint32_t memtop = rom::MemTop;
    // we do not want to overwrite ROM variables or even the vector table
    memstart = std::max(memstart, (uint32_t)0x1000);

    static macboot_memmap_response e3 = {
        memstart,              // base
        BL_BEGIN - memstart,   // size
        MACBOOT_MEMMAP_USABLE, // type
        nullptr                // next
    };
    static macboot_memmap_response e2 = {
        BL_BEGIN,                              // base
        BL_END - BL_BEGIN,                     // size
        MACBOOT_MEMMAP_BOOTLOADER_RECLAIMABLE, // type
        &e3                                    // next
    };
    static macboot_memmap_response e1 = {
        BL_END,                // base
        memtop - BL_END,       // size
        MACBOOT_MEMMAP_USABLE, // type
        &e2                    // next
    };

    return &e1;
}

extern "C" void _kernelentry(uint32_t sp, uint32_t kjmp); // kernelentry.S

void boot() {
    struct fat_driver_state fat;
    if (init_fat_driver(
            &fat,
            [](void *buf, uint32_t count, uint32_t offset) {
                if (!disk::read(offset, buf, count)) {
                    screen::puts("ERR   disk read error\n");
                }
            },
            BL_END - BL_BEGIN) != 0) {
        screen::puts("ERR   could not initialize FAT\n");
        return;
    }
    screen::puts("OK    FAT initialized\n");

    struct fat_file_handle config_file;
    if (!fat_open_file(&fat, &config_file, "boot.cfg")) {
        screen::puts("ERR   could not find config file\n");
        return;
    }

    char kernelpath[30];
    if (!get_cfg_value(&fat, &config_file, "KERNEL_PATH", kernelpath, sizeof(kernelpath))) {
        screen::puts("ERR   could not get KERNEL_PATH config value\n");
        return;
    }

    struct fat_file_handle kernel_file;
    if (!fat_open_file(&fat, &kernel_file, kernelpath)) {
        screen::puts("ERR   could not find kernel\n");
        return;
    }
    screen::puts("OK    opened kernel file\n");

    struct macboot_kernel_header kheader;
    if (fat_read(&fat, &kernel_file, &kheader, sizeof(kheader)) != sizeof(kheader)) {
        screen::puts("ERR   error reading kernel file\n");
        return;
    }

    if (kheader.id[0] != MAGIC1 || kheader.id[1] != MAGIC3) {
        screen::puts("ERR   kernel magic mismatch\n");
        return;
    }

    uint32_t kernelfsize = fat_seek(&fat, &kernel_file, 0, FAT_SEEK_END);

    // check if kernel load address conflicts with bootloader space or is outside of RAM
    if (((kheader.load_address < BL_END) && ((kheader.load_address + kheader.size) > BL_BEGIN)) || (kheader.load_address < rom::RAMBase) ||
        (((kheader.load_address + kheader.size)) > rom::MemTop)) {
        screen::puts("ERR   kernel load address is not in usable memory\n");
        return;
    }

    if (kernelfsize > kheader.size) {
        screen::puts("ERR   kernel file is larger than reported by header\n");
        return;
    }

    memset((void *)kheader.load_address, 0, kheader.size); // set kernel .bss etc. to zero
    fat_seek(&fat, &kernel_file, 0, FAT_SEEK_BEGIN);
    if (fat_read(&fat, &kernel_file, (void *)kheader.load_address, kernelfsize) != kernelfsize) {
        screen::puts("ERR   error reading kernel from disk\n");
        return;
    }

    // process all requests
    for (uint32_t i = 0; i < kernelfsize; i++) {
        uint64_t *ptr = (uint64_t *)(kheader.load_address + i);
        if ((ptr[0] != MAGIC1) || ((ptr[1] & 0xFFFFFFFF00000000) != MAGIC2(0)) || (ptr[2] != MAGIC3)) {
            continue;
        }

        uint32_t id = ptr[1] & 0xFFFFFFFF;
        switch (id) {
        case 1: { // kmem
            struct macboot_kmem_request *rq = (struct macboot_kmem_request *)ptr;
            struct macboot_kmem_response *rs = (struct macboot_kmem_response *)bump_malloc(sizeof(struct macboot_kmem_response));
            if (rs == nullptr) {
                screen::puts("ERR   out of memory\n");
                return;
            }
            rq->response = rs;
            rs->base = kheader.load_address;
            rs->size = kheader.size;
            break;
        }
        case 2: { // memmap
            struct macboot_memmap_request *rq = (struct macboot_memmap_request *)ptr;
            rq->response = gen_memmap();
            break;
        }
        case 3: { // framebuffer
            struct macboot_framebuffer_request *rq = (struct macboot_framebuffer_request *)ptr;
            struct macboot_framebuffer_response *rs = (struct macboot_framebuffer_response *)bump_malloc(sizeof(struct macboot_framebuffer_response));
            if (rs == nullptr) {
                screen::puts("ERR   out of memory\n");
                return;
            }
            rq->response = rs;
            rs->base = (uint32_t)screen::fb.base;
            rs->width = screen::fb.width;
            rs->height = screen::fb.height;
            rs->pitch = screen::fb.pitch;
            rs->bpp = screen::fb.bpp;
            break;
        }
        case 4: { // cmdline
            char *buf = (char *)bump_malloc(100);
            if (buf == nullptr) {
                screen::puts("ERR   out of memory\n");
                return;
            }
            if (!get_cfg_value(&fat, &config_file, "CMDLINE", buf, 100)) {
                bump_dealloc(100);
                break;
            }
            struct macboot_cmdline_request *rq = (struct macboot_cmdline_request *)ptr;
            struct macboot_cmdline_response *rs = (struct macboot_cmdline_response *)bump_malloc(sizeof(struct macboot_cmdline_response));
            if (rs == nullptr) {
                screen::puts("ERR   out of memory\n");
                return;
            }
            rq->response = rs;
            rs->cmdline = buf;
            break;
        }
        case 5: { // terminal
            struct macboot_terminal_request *rq = (struct macboot_terminal_request *)ptr;
            struct macboot_terminal_response *rs = (struct macboot_terminal_response *)bump_malloc(sizeof(struct macboot_terminal_response));
            if (rs == nullptr) {
                screen::puts("ERR   out of memory\n");
                return;
            }
            rq->response = rs;
            rs->puts = &screen::puts;
            break;
        }
        }
    }

    screen::puts("OK    jumping to kernel\n");
    void *newsp = bump_malloc(1024);
    if (newsp == nullptr) {
        screen::puts("ERR   out of memory\n");
        return;
    }
    uint32_t sp = (uint32_t)newsp;
    sp += 1024; // stack grows downwards
    _kernelentry(sp, (uint32_t)kheader.kmain);
}
