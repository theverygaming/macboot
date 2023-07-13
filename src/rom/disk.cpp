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
    if (offset % 512 != 0) {
        return false;
    }

    struct rom_param_block b;
    b = {
        {0},            // _unused0
        rom::BootDrive, // ioVRefNum
        rom::BtDskRfn,  // ioRefNum
        {0},            // _unused1
        1,              // ioPermssn -  read-only permission
        {0},            // _unused2
        (uint32_t)buf,  // ioBuffer
        blocks * 512,   // ioReqCount
        0,              // ioActCount
        1,              // ioPosMode - read from start of disk
        offset,         // ioPosOffset
    };
    return (_Read(&b) == 0 && b.ioActCount == blocks * 512);
}
