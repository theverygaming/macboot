#include <boot/boot.h>
#include <boot/config.h>
#include <bumpalloc.h>
#include <disk.h>
#include <fat/fat.h>
#include <screen.h>
#include <stdlib.h>
#include <types.h>
#include <memmap.h>

static struct littleboot_memmap_response *gen_memmap() {
    struct littleboot_memmap_response *first = nullptr;
    struct littleboot_memmap_response *prev = nullptr;
    paddr_t base;
    size_t size;
    int type;
    void *ctx = nullptr;
    while (memmap::iter(&base, &size, &type, &ctx)) {
        struct littleboot_memmap_response *current = (struct littleboot_memmap_response *)bump_malloc(sizeof(struct littleboot_memmap_response));
        current->base = base;
        current->size = size;
        current->type = type;
        current->next = nullptr;
        if (first == nullptr) {
            first = current;
        }
        if (prev != nullptr) {
            prev->next = current;
        }
        prev = current;
    }

    return first;
}

void boot() {
    struct fat_driver_state fat;
    if (init_fat_driver(
            &fat,
            [](void *buf, uint32_t count, uint32_t offset) {
                if (!disk::read(offset, buf, count)) {
                    screen::puts("ERR   disk read error\n");
                }
            },
            disk::get_boot_fat_offset()) != 0) {
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

    struct littleboot_kernel_header kheader;
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
    paddr_t base;
    size_t size;
    int type;
    void *ctx = nullptr;
    bool within_usable_mem = false;
    while (memmap::iter(&base, &size, &type, &ctx)) {
        if (type != LITTLEBOOT_MEMMAP_USABLE) {
            continue;
        }

        if ((kheader.load_address >= base) && (kheader.load_end_address < (base + size))) {
            within_usable_mem = true;
        }
    }
    if (!within_usable_mem) {
        screen::puts("ERR   kernel load address is not in usable memory\n");
        return;
    }

    if (kernelfsize > (kheader.load_end_address - kheader.load_address)) {
        screen::puts("ERR   kernel file is larger than reported by header\n");
        return;
    }

    memset((void *)kheader.load_address, 0, kheader.load_end_address - kheader.load_address); // set kernel .bss etc. to zero
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
            struct littleboot_kmem_request *rq = (struct littleboot_kmem_request *)ptr;
            struct littleboot_kmem_response *rs = (struct littleboot_kmem_response *)bump_malloc(sizeof(struct littleboot_kmem_response));
            if (rs == nullptr) {
                screen::puts("ERR   out of memory\n");
                return;
            }
            rq->response = rs;
            rs->base = kheader.load_address;
            rs->size = kheader.load_end_address - kheader.load_address;
            break;
        }
        case 2: { // memmap
            struct littleboot_memmap_request *rq = (struct littleboot_memmap_request *)ptr;
            rq->response = gen_memmap();
            break;
        }
        case 3: { // framebuffer
            struct littleboot_framebuffer_request *rq = (struct littleboot_framebuffer_request *)ptr;
            struct littleboot_framebuffer_response *rs = (struct littleboot_framebuffer_response *)bump_malloc(sizeof(struct littleboot_framebuffer_response));
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
            struct littleboot_cmdline_request *rq = (struct littleboot_cmdline_request *)ptr;
            struct littleboot_cmdline_response *rs = (struct littleboot_cmdline_response *)bump_malloc(sizeof(struct littleboot_cmdline_response));
            if (rs == nullptr) {
                screen::puts("ERR   out of memory\n");
                return;
            }
            rq->response = rs;
            rs->cmdline = buf;
            break;
        }
        case 5: { // terminal
            struct littleboot_terminal_request *rq = (struct littleboot_terminal_request *)ptr;
            struct littleboot_terminal_response *rs = (struct littleboot_terminal_response *)bump_malloc(sizeof(struct littleboot_terminal_response));
            if (rs == nullptr) {
                screen::puts("ERR   out of memory\n");
                return;
            }
            rq->response = rs;
            rs->puts = &screen::puts;
            break;
        }
        case 6: { // module
            struct littleboot_module_request *rq = (struct littleboot_module_request *)ptr;
            struct littleboot_module_response *prev_rs = nullptr;

            size_t seek_pos = 0;
            char modulepath[30];
            while (get_cfg_value(&fat, &config_file, "MODULE_PATH", modulepath, sizeof(modulepath), &seek_pos)) {
                screen::puts("OK    loading module: ");
                screen::puts(modulepath);
                screen::puts("\n");

                struct fat_file_handle module_file;
                if (!fat_open_file(&fat, &module_file, modulepath)) {
                    screen::puts("ERR   could not find module\n");
                    return;
                }
                screen::puts("OK    opened module file\n");

                size_t data_size = fat_seek(&fat, &module_file, 0, FAT_SEEK_END);
                void *data = bump_malloc(data_size);
                if (data == nullptr) {
                    screen::puts("ERR   out of memory\n");
                    return;
                }

                fat_seek(&fat, &module_file, 0, FAT_SEEK_BEGIN);
                if (fat_read(&fat, &module_file, data, data_size) != data_size) {
                    screen::puts("ERR   error reading module from disk\n");
                    return;
                }

                struct littleboot_module_response *rs = (struct littleboot_module_response *)bump_malloc(sizeof(struct littleboot_module_response));
                if (rs == nullptr) {
                    screen::puts("ERR   out of memory\n");
                    return;
                }
                rs->address = data;
                rs->size = data_size;
                rs->next = nullptr;
                if (prev_rs != nullptr) {
                    prev_rs->next = rs;
                } else {
                    rq->response = rs;
                }
                prev_rs = rs;
            }
            break;
        }
        }
    }

    screen::puts("OK    preparing kernel entry\n");
    enter_kernel(&kheader);
}
