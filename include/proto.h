#ifndef _PROTO_H
#define _PROTO_H

#ifndef LITTLEBOOT_HAS_STDINT
#include <stdint.h>
#endif

#define MAGIC1     (0x66786F6573203A33)
#define MAGIC2(id) (0x6375746500000000 | (id))
#define MAGIC3     (0x666C756666792C2C)

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t littleboot_size_t;
typedef uint32_t littleboot_uintptr_t;
typedef uint32_t littleboot_paddr_t;

#define LITTLEBOOT_KERNEL_HEADER_ID \
    { MAGIC1, MAGIC3 }
struct __attribute__((packed)) littleboot_kernel_header {
    uint64_t id[2];
    littleboot_uintptr_t load_address; // physical address where kernel executable will be loaded
    littleboot_uintptr_t load_end_address; // physical end address of kernel executable (including .bss) - NOT the size on disk
    void (*kmain)();
};

struct __attribute__((packed)) littleboot_kmem_response {
    littleboot_uintptr_t base;
    littleboot_size_t size;
};

#define LITTLEBOOT_KMEM_REQUEST_ID \
    { MAGIC1, MAGIC2(1), MAGIC3 }
struct __attribute__((packed)) littleboot_kmem_request {
    uint64_t id[3];
    struct littleboot_kmem_response *response;
};

#define LITTLEBOOT_MEMMAP_USABLE                 (1)
#define LITTLEBOOT_MEMMAP_RESERVED               (2)
#define LITTLEBOOT_MEMMAP_BOOTLOADER_RECLAIMABLE (3)
struct __attribute__((packed)) littleboot_memmap_response {
    littleboot_paddr_t base;
    littleboot_size_t size;
    int type;
    struct littleboot_memmap_response *next;
};

#define LITTLEBOOT_MEMMAP_REQUEST_ID \
    { MAGIC1, MAGIC2(2), MAGIC3 }
struct __attribute__((packed)) littleboot_memmap_request {
    uint64_t id[3];
    struct littleboot_memmap_response *response;
};

struct __attribute__((packed)) littleboot_framebuffer_response {
    littleboot_uintptr_t base;
    littleboot_size_t width;
    littleboot_size_t height;
    littleboot_size_t pitch;
    unsigned short bpp;
};

#define LITTLEBOOT_FRAMEBUFFER_REQUEST_ID \
    { MAGIC1, MAGIC2(3), MAGIC3 }
struct __attribute__((packed)) littleboot_framebuffer_request {
    uint64_t id[3];
    struct littleboot_framebuffer_response *response;
};

struct __attribute__((packed)) littleboot_cmdline_response {
    const char *cmdline;
};

#define LITTLEBOOT_CMDLINE_REQUEST_ID \
    { MAGIC1, MAGIC2(4), MAGIC3 }
struct __attribute__((packed)) littleboot_cmdline_request {
    uint64_t id[3];
    struct littleboot_cmdline_response *response;
};

struct __attribute__((packed)) littleboot_terminal_response {
    void (*puts)(const char *str);
};

#define LITTLEBOOT_TERMINAL_REQUEST_ID \
    { MAGIC1, MAGIC2(5), MAGIC3 }
struct __attribute__((packed)) littleboot_terminal_request {
    uint64_t id[3];
    struct littleboot_terminal_response *response;
};

#ifdef __cplusplus
}
#endif

#endif /* _PROTO_H */
