#pragma once
#include <fat/endianness.h>
#include <types.h>

#define _SWAPFIELD(s, f) (s).f = swap_le((s).f)

struct __attribute((packed)) fat_bpb {
    uint8_t jump[3];
    int8_t oem_id[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;
    uint16_t root_dir_ents;
    uint16_t sector_count;    // if 0 then actual sector count is stored in large sector count field
    uint8_t media_descriptor;
    uint16_t sectors_per_fat; // FAT12 & 16 only
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sector_count;
    uint32_t large_sector_count;
};

#define SWAP_fat_bpb(x)                 \
    _SWAPFIELD(x, bytes_per_sector);    \
    _SWAPFIELD(x, reserved_sectors);    \
    _SWAPFIELD(x, root_dir_ents);       \
    _SWAPFIELD(x, sector_count);        \
    _SWAPFIELD(x, sectors_per_fat);     \
    _SWAPFIELD(x, sectors_per_track);   \
    _SWAPFIELD(x, head_count);          \
    _SWAPFIELD(x, hidden_sector_count); \
    _SWAPFIELD(x, large_sector_count);

struct __attribute((packed)) fat_ebpb_12_16 {
    uint8_t drive_number;
    uint8_t nt_flags;
    uint8_t signature;
    uint32_t volume_id;
    int8_t volume_label[11];
    int8_t fat_system_id[8];
};

#define SWAP_fat_ebpb_12_16(x) _SWAPFIELD(x, volume_id)

struct __attribute((packed)) fat_ebpb_32 {
    uint32_t sectors_per_fat;
    uint16_t flags;
    uint16_t fat_version;
    uint32_t root_cluster_number;
    uint16_t fsinfo_sector;
    uint16_t bootbackup_sector;
    uint8_t _reserved1[12];

    struct fat_ebpb_12_16 ebpb_12_16;
};

#define SWAP_fat_ebpb_32(x)             \
    _SWAPFIELD(x, sectors_per_fat);     \
    _SWAPFIELD(x, flags);               \
    _SWAPFIELD(x, fat_version);         \
    _SWAPFIELD(x, root_cluster_number); \
    _SWAPFIELD(x, fsinfo_sector);       \
    _SWAPFIELD(x, bootbackup_sector);   \
    SWAP_fat_ebpb_12_16((x).ebpb_12_16)

struct __attribute((packed)) fat_83_dirent {
    int8_t name[8];
    int8_t extension[3];
    uint8_t attr;
    uint8_t nt;
    uint8_t creation_time_fine;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t cluster_high;
    uint16_t modification_time;
    uint16_t modification_date;
    uint16_t cluster_low;
    uint32_t filesize;
};

#define DIRENT_READ_ONLY (0x01)
#define DIRENT_HIDDEN    (0x02)
#define DIRENT_SYSTEM    (0x04)
#define DIRENT_VOLUME_ID (0x08)
#define DIRENT_DIRECTORY (0x10)
#define DIRENT_ARCHIVE   (0x20)
#define DIRENT_LONG_NAME (DIRENT_READ_ONLY | DIRENT_HIDDEN | DIRENT_SYSTEM | DIRENT_VOLUME_ID)

#define SWAP_fat_83_dirent(x)         \
    _SWAPFIELD(x, creation_time);     \
    _SWAPFIELD(x, creation_date);     \
    _SWAPFIELD(x, access_date);       \
    _SWAPFIELD(x, cluster_high);      \
    _SWAPFIELD(x, modification_time); \
    _SWAPFIELD(x, modification_date); \
    _SWAPFIELD(x, cluster_low);       \
    _SWAPFIELD(x, filesize)
