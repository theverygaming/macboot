#include <boot.h>
#include <rom/disk.h>
#include <rom/globals.h>
#include <screen.h>
#include <stdlib.h>
#include <types.h>

static bool disk_byte_read(uint32_t offset, void *buf, size_t n) {
    int sectors = n / 512;
    if (!rom::read_boot_disk(offset, buf, sectors)) {
        return false;
    }
    n -= 512 * sectors;
    offset += 512 * sectors;
    buf = (void *)((uint32_t)buf + (512 * sectors));

    static uint8_t buf_l[512];
    bool s = rom::read_boot_disk(offset, buf_l, 1);
    memcpy(buf, buf_l, n);
    return s;
}

// #define MAGIC (0x666F7073)
#define MAGIC (0x66786F6573203A33)

struct memmap_entry {
    uint32_t base;
    uint32_t size;
    enum class type {
        USABLE,
        RESERVED,
        BAD_MEMORY,
        BOOTLOADER_RECLAIMABLE,
        KERNEL,
        MMIO,
    } type;
    struct memmap_entry *next;
};

struct bootloaderinfo {
    struct memmap_entry *memmap_first;
    struct screen::fbinfo *fbinfo;
};

struct module {
    uint32_t disk_offset;
    uint32_t size; // no module present if zero
};

struct bootheader {
    uint64_t magic;
    uint32_t load_adr;              // physical address to load kernel at
    uint32_t size;                  // kernel size (including .bss)
    uint32_t disksize;              // kernel size on disk
    void (*kmain)(uint32_t modadr); // kernel init function (if modaddr argument is zero then no module present)
    struct module mod;
    struct bootloaderinfo info;
};

static struct memmap_entry *gen_memmap(uint32_t kstart, uint32_t kend) {
    // would use malloc later but for now this will work perfectly fine
    static memmap_entry e3{
        .base = kstart,
        .size = kend - kstart,
        .type = memmap_entry::type::KERNEL,
        .next = nullptr};
    static memmap_entry e2{
        .base = 0x6000,
        .size = 0xB000,
        .type = memmap_entry::type::BOOTLOADER_RECLAIMABLE,
        .next = &e3};
    static memmap_entry e1{
        .base = *rom::RAMBase, // below 0x1000 there are various variables from the ROM we don't want to touch
        .size = *rom::MemTop - 0x1000,
        .type = memmap_entry::type::USABLE,
        .next = &e2};
    if (e1.base < 0x1000) {
        e1.base = 0x1000;
    }

    return &e1;
}

void boot() {
    struct bootheader header;
    if (!disk_byte_read(4096, &header, sizeof(struct bootheader))) {
        screen::puts("ERR   error reading from disk\n");
        return;
    }
    screen::puts("OK    loaded header\n");
    if (header.magic != MAGIC) {
        screen::puts("ERR   magic does not match\n");
        return;
    }
    screen::puts("OK    magic\n");

    if ((header.load_adr < 0x11000 && header.load_adr >= 0x6000) || (header.load_adr + header.size < 0x11000 && header.load_adr + header.size >= 0x6000)) {
        screen::puts("ERR   invalid kernel load address\n");
        return;
    }

    uint32_t kend = header.load_adr + header.size;
    if (header.mod.disk_offset != 0) {
        kend += header.mod.size;
    }
    if (kend >= *rom::MemTop) {
        screen::puts("ERR   not enough memory to load kernel\n");
        return;
    }

    if (!disk_byte_read(4096, (void *)header.load_adr, header.disksize)) {
        screen::puts("ERR   failed to load kernel from disk\n");
        return;
    }
    screen::puts("OK    kernel loaded\n");

    struct bootheader *kheader = (struct bootheader *)header.load_adr;

    uint32_t module_adr = 0;

    if (header.mod.size != 0) {
        if ((header.load_adr + header.size < 0x11000 && header.load_adr + header.size >= 0x6000) || (header.load_adr + header.size + header.mod.size < 0x11000 && header.load_adr + header.size + header.mod.size >= 0x6000)) {
            screen::puts("ERR   invalid module load address\n");
        } else {
            if (!disk_byte_read(4096 + header.disksize, (void *)(header.load_adr + header.size), header.mod.size)) {
                screen::puts("ERR   failed to load module from disk\n");
            } else {
                module_adr = header.load_adr + header.size;
                screen::puts("OK    module loaded\n");
            }
        }
    }

    kend = header.load_adr + header.size;
    if (module_adr != 0) {
        kend += header.mod.size;
    }

    kheader->info.memmap_first = gen_memmap(header.load_adr, kend);
    kheader->info.fbinfo = &screen::fb;
    screen::puts("OK    generated memory map\n");

    if ((uint32_t)header.kmain < header.load_adr || (uint32_t)header.kmain > header.load_adr + header.disksize) {
        screen::puts("ERR   invalid kmain address\n");
        return;
    }

    screen::puts("OK    calling kernel\n");
    header.kmain(module_adr);
    screen::puts("ERR   kmain returned\n");
}
