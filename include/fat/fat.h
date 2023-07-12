#pragma once
#include <fat/fatstructs.h>
#include <types.h>

struct fat_driver_state {
    void (*read_disk)(void *buf, size_t count, size_t offset);
    unsigned int fat_type;
    size_t disk_part_start;

    uint32_t fat_start_sector;
    uint32_t fat_size; // in sectors
    uint32_t rootdir_start_sector;

    uint32_t fat32_root_cluster;

    struct fat_bpb bpb;
};

struct fat_file_handle {
    struct fat_83_dirent dirent;
    uint32_t seekpos;
    bool open;
};

// returns nonzero value on error
int init_fat_driver(struct fat_driver_state *ctx, void (*read_disk)(void *buf, uint32_t count, uint32_t offset), uint32_t disk_part_start);

// returns false on error
bool fat_open_file(struct fat_driver_state *ctx, struct fat_file_handle *file, const char *path);

#define FAT_SEEK_CURRENT (0x01)
#define FAT_SEEK_BEGIN   (0x02)
#define FAT_SEEK_END     (0x04)
uint32_t fat_seek(struct fat_driver_state *ctx, struct fat_file_handle *file, int32_t pos, uint8_t flags = FAT_SEEK_CURRENT);

uint32_t fat_read(struct fat_driver_state *ctx, struct fat_file_handle *file, void *buf, uint32_t count);
