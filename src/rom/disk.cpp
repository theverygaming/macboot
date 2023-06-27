#include <rom/disk.h>
#include <rom/globals.h>
#include <types.h>

struct __attribute__((packed)) rom_param_block {
    uint8_t _unused0[22];
    uint16_t ioVRefNum;
    uint16_t ioRefNum;
    uint8_t _unused1[1];
    uint8_t ioPermssn;
    uint8_t _unused2[4];
    uint32_t ioBuffer;
    uint32_t ioReqCount;
    uint32_t ioActCount;
    uint16_t ioPosMode;
    uint32_t ioPosOffset;
};

extern "C" int16_t _Read(struct rom_param_block *param);

bool rom::read_boot_disk(uint32_t offset, void *buf, uint32_t blocks) {
    if (blocks == 0) {
        return true;
    }

    struct rom_param_block b = {
        ._unused0 = {0},
        .ioVRefNum = *BootDrive,
        .ioRefNum = *BtDskRfn,
        ._unused1 = {0},
        .ioPermssn = 1, // read-only permission
        ._unused2 = {0},
        .ioBuffer = (uint32_t)buf,
        .ioReqCount = blocks * 512,
        .ioActCount = 0,
        .ioPosMode = 0x0001, // read from start of disk
        .ioPosOffset = offset,
    };
    return (_Read(&b) == 0 && b.ioActCount == blocks * 512);
}
