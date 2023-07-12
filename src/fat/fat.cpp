#include <algorithm>
#include <fat/endianness.h>
#include <fat/fat.h>
#include <fat/fatstructs.h>
#include <stdlib.h>

// assuming name = 8 bytes and extension = 3 bytes - returns true if filename was successfully split
static bool split_file_name(const char *path, char *name, char *extension, int n) {
    memset(name, ' ', 8);
    memset(extension, ' ', 3);

    int offset = 0;
    int end = 0;

    while (path[offset] == '/') {
        offset++;
    }

    for (int i = 0; i < n; i++) {
        while (path[offset] == '/') {
            offset++;
        }
        while ((path[offset] != '/') && (path[offset] != '\0')) {
            offset++;
        }
        while (path[offset] == '/') {
            offset++;
        }
    }
    // find next / or null terminator
    end = offset;
    while ((path[end] != '/') && (path[end] != '\0')) {
        end++;
    }

    if ((end - 1) == offset) { // reached end of path
        return false;
    }

    // find the extension
    size_t fname_size = end - offset;
    size_t ext_size = 0;
    const char *extptr = nullptr;
    for (int i = 0; i < (end - offset); i++) {
        if (path[offset + i] == '.') {
            extptr = &path[offset + i + 1];
            fname_size = i;
            ext_size = (end - offset) - i - 1;
        }
    }
    if (extptr != nullptr) {
        strncpy(extension, extptr, std::min(ext_size, (size_t)3));
    }
    strncpy(name, &path[offset], std::min(fname_size, (size_t)8));

    return true;
}

static char fat_charconvert(char in) {
    // convert to uppercase
    if (in >= 'a' && in <= 'z') {
        in -= 'a' - 'A';
    }

    return in;
}

// returns true if equal
static bool fat_namecmp(const char *a, const char *b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (fat_charconvert(a[i]) != fat_charconvert(b[i])) {
            return false;
        }
    }
    return true;
}

// assuming name = 8 bytes and extension = 3 bytes - returns true if equal
static bool fat_namecmp(const char *name_a, const char *extension_a, const char *name_b, const char *extension_b) {
    return fat_namecmp(name_a, name_b, 8) && fat_namecmp(extension_a, extension_b, 3);
}

// returns false if end of cluster chain has been reached
static bool fat_get_next_cluster(struct fat_driver_state *ctx, uint32_t cluster, uint32_t *next) {
    uint32_t fat_byte_offset = 0;
    switch (ctx->fat_type) {
    case 12:
        fat_byte_offset = cluster + (cluster / 2); // * 1.5
        break;
    case 16:
        fat_byte_offset = cluster * 2;
        break;
    case 32:
        fat_byte_offset = cluster * 4;
        break;
    }
    uint32_t fat_sector = ctx->fat_start_sector + (fat_byte_offset / ctx->bpb.bytes_per_sector);
    uint32_t sector_byte_offset = fat_byte_offset % ctx->bpb.bytes_per_sector;

    *next = 0;

    switch (ctx->fat_type) {
    case 12: {
        uint16_t value;
        ctx->read_disk(&value, sizeof(value), ctx->disk_part_start + (fat_sector * ctx->bpb.bytes_per_sector) + sector_byte_offset);
        value = swap_le(value);
        if (cluster & 0x1) { // is this an odd cluster?
            *next = value >> 4;
        } else {
            *next = value & 0xFFF;
        }
        return *next < 0xFF8;
    }
    case 16: {
        uint16_t value;
        ctx->read_disk(&value, sizeof(value), ctx->disk_part_start + (fat_sector * ctx->bpb.bytes_per_sector) + sector_byte_offset);
        value = swap_le(value);
        *next = value;
        return *next < 0xFFF8;
    }
    case 32: {
        uint32_t value;
        ctx->read_disk(&value, sizeof(value), ctx->disk_part_start + (fat_sector * ctx->bpb.bytes_per_sector) + sector_byte_offset);
        value = swap_le(value);
        *next = value;
        return *next < 0x0FFFFFF8;
    }
    }
    return false;
}

static uint32_t fat_cluster_to_sector(struct fat_driver_state *ctx, uint32_t cluster) {
    return ((cluster - 2) * ctx->bpb.sectors_per_cluster) +
           (ctx->bpb.reserved_sectors + (ctx->bpb.fat_count * ctx->fat_size) +
            (((ctx->bpb.root_dir_ents * sizeof(struct fat_83_dirent)) + (ctx->bpb.bytes_per_sector - 1)) / ctx->bpb.bytes_per_sector));
}

static uint32_t get_sector_count(struct fat_driver_state *ctx) {
    return ctx->bpb.sector_count == 0 ? ctx->bpb.large_sector_count : ctx->bpb.sector_count;
}

static unsigned int detect_fat_type(struct fat_driver_state *ctx) {
    // read EBPB
    struct fat_ebpb_32 ebpb;
    ctx->read_disk(&ebpb, sizeof(ebpb), ctx->disk_part_start + sizeof(ctx->bpb));
    SWAP_fat_ebpb_32(ebpb);

    uint32_t sectors_per_fat = ctx->bpb.sectors_per_fat;
    if (sectors_per_fat == 0) {
        sectors_per_fat = ebpb.sectors_per_fat;
    }

    uint32_t root_dir_sectors =
        (((uint32_t)ctx->bpb.root_dir_ents * sizeof(struct fat_83_dirent)) + ((uint32_t)ctx->bpb.bytes_per_sector - 1)) / ctx->bpb.bytes_per_sector;
    uint32_t data_sectors =
        get_sector_count(ctx) - ((uint32_t)ctx->bpb.reserved_sectors + ((uint32_t)ctx->bpb.fat_count * sectors_per_fat) + root_dir_sectors);
    uint32_t total_clusters = data_sectors;
    if (ctx->bpb.sectors_per_cluster > 1) {
        total_clusters /= ctx->bpb.sectors_per_cluster;
    }
    if (total_clusters < 4085) {
        return 12;
    } else if (total_clusters < 65525) {
        return 16;
    } else {
        return 32;
    }
}

int init_fat_driver(struct fat_driver_state *ctx, void (*read_disk)(void *buf, uint32_t count, uint32_t offset), uint32_t disk_part_start) {
    ctx->read_disk = read_disk;
    ctx->disk_part_start = disk_part_start;

    // read BPB
    ctx->read_disk(&ctx->bpb, sizeof(ctx->bpb), ctx->disk_part_start + 0);
    SWAP_fat_bpb(ctx->bpb);

    // bpb sanity checks
    if ((ctx->bpb.sectors_per_cluster == 0) || (ctx->bpb.bytes_per_sector == 0) ||
        ((ctx->bpb.bytes_per_sector & (ctx->bpb.bytes_per_sector - 1)) != 0)) {
        return -1;
    }

    ctx->fat_type = detect_fat_type(ctx);

    // check if fat type makes sense and if the BPB values are sane
    if (ctx->fat_type == 32) {
        if (ctx->bpb.sectors_per_fat != 0 || ctx->bpb.root_dir_ents != 0) {
            return -2;
        }
    } else {
        if (ctx->bpb.sectors_per_fat == 0) {
            return -2;
        }
    }
    if (ctx->bpb.bytes_per_sector == 0) {
        return -3;
    }

    ctx->fat_start_sector = ctx->bpb.reserved_sectors;

    if (ctx->fat_type == 32) {
        struct fat_ebpb_32 ebpb;
        ctx->read_disk(&ebpb, sizeof(ebpb), ctx->disk_part_start + sizeof(ctx->bpb));
        SWAP_fat_ebpb_32(ebpb);
        ctx->fat_size = ebpb.sectors_per_fat;
        ctx->fat32_root_cluster = ebpb.root_cluster_number;
    } else {
        ctx->fat_size = ctx->bpb.sectors_per_fat;
    }
    ctx->rootdir_start_sector = ctx->bpb.reserved_sectors + (ctx->bpb.fat_count * ctx->fat_size);

    // final sanity checks
    if (ctx->fat_size == 0) {
        return -4;
    }

    return 0;
}

bool fat_open_file(struct fat_driver_state *ctx, struct fat_file_handle *file, const char *path) {
    file->open = false;
    char name[8];
    char extension[3];
    int path_counter = 0;

    uint32_t dir_current_cluster = ctx->fat32_root_cluster;
    uint32_t dir_current_cluster_index = 0;

    bool is_root_dir = (ctx->fat_type == 12) || (ctx->fat_type == 16);
    uint32_t root_dir_start = (ctx->rootdir_start_sector * ctx->bpb.bytes_per_sector);
    uint32_t root_dir_ents = ctx->bpb.root_dir_ents;
    uint32_t root_dir_counter = 0;

    while (true) {
        struct fat_83_dirent ent;
        bool found = false;
        while (is_root_dir ? root_dir_counter < root_dir_ents : true) {
            if (is_root_dir) {
                ctx->read_disk(&ent, sizeof(ent), ctx->disk_part_start + root_dir_start + (root_dir_counter * sizeof(ent)));
                SWAP_fat_83_dirent(ent);
                root_dir_counter++;
            } else {
                ctx->read_disk(&ent,
                               sizeof(ent),
                               ctx->disk_part_start + (fat_cluster_to_sector(ctx, dir_current_cluster) * ctx->bpb.bytes_per_sector) +
                                   (dir_current_cluster_index * sizeof(ent)));
                SWAP_fat_83_dirent(ent);
                dir_current_cluster_index++;
            }

            if (ent.name[0] == 0xE5) { // free dirent
                goto cont;
            }
            if (ent.name[0] == 0) { // last dirent
                break;
            }
            if (ent.attr & DIRENT_LONG_NAME) { // we do not do long file names
                goto cont;
            }

            if (!split_file_name(path, name, extension, path_counter)) {
                break;
            }

            if (fat_namecmp(name, extension, (char *)ent.name, (char *)ent.extension)) {
                found = true;
                path_counter++;
                if (ent.attr & DIRENT_DIRECTORY) {
                    dir_current_cluster = ent.cluster_low | ((uint32_t)ent.cluster_high << 16);
                    dir_current_cluster_index = 0;
                    is_root_dir = false;
                }
                break;
            }

        cont:
            if (!is_root_dir && dir_current_cluster_index >= (((uint32_t)ctx->bpb.bytes_per_sector * ctx->bpb.sectors_per_cluster) / sizeof(ent))) {
                dir_current_cluster_index = 0;
                if (!fat_get_next_cluster(ctx, dir_current_cluster, &dir_current_cluster)) {
                    break;
                }
            }
        }
        if (found) {
            if (ent.attr & DIRENT_DIRECTORY) {
                continue;
            }
            file->dirent = ent;
            file->seekpos = 0;
            file->open = true;
            return true;

        } else {
            return false; // we didn't find the file
        }
    }

    return false;
}

uint32_t fat_seek(struct fat_driver_state *ctx, struct fat_file_handle *file, int32_t pos, uint8_t flags) {
    if (!file->open) {
        return 0;
    }
    if (flags & FAT_SEEK_CURRENT) {
        file->seekpos += pos;
    } else if (flags & FAT_SEEK_END) {
        file->seekpos = file->dirent.filesize;
        file->seekpos += pos;
    } else if (flags & FAT_SEEK_BEGIN) {
        file->seekpos = pos;
    }

    file->seekpos = std::min(file->dirent.filesize, file->seekpos);
    return file->seekpos;
}

uint32_t fat_read(struct fat_driver_state *ctx, struct fat_file_handle *file, void *_buf, uint32_t count) {
    if (!file->open) {
        return 0;
    }
    uint8_t *buf = (uint8_t *)_buf;
    if ((file->seekpos + count) > file->dirent.filesize) {
        count = file->dirent.filesize - file->seekpos;
    }

    uint32_t current_cluster = file->dirent.cluster_low | ((uint32_t)file->dirent.cluster_high << 16);
    for (uint32_t i = 0; i < (file->seekpos / ((uint32_t)ctx->bpb.sectors_per_cluster * ctx->bpb.bytes_per_sector)); i++) {
        if (!fat_get_next_cluster(ctx, current_cluster, &current_cluster)) {
            return 0;
        }
    }
    uint32_t current_cluster_offset = file->seekpos % ((uint32_t)ctx->bpb.sectors_per_cluster * ctx->bpb.bytes_per_sector);

    uint32_t leftover_read_count = count;
    while (leftover_read_count != 0) {
        if (current_cluster_offset >= ((uint32_t)ctx->bpb.sectors_per_cluster * ctx->bpb.bytes_per_sector)) {
            if (!fat_get_next_cluster(ctx, current_cluster, &current_cluster)) {
                return count - leftover_read_count;
            }
            current_cluster_offset = 0;
        }
        uint32_t read_amount =
            std::min(leftover_read_count, ((uint32_t)ctx->bpb.sectors_per_cluster * ctx->bpb.bytes_per_sector) - current_cluster_offset);

        ctx->read_disk(buf + (count - leftover_read_count),
                       read_amount,
                       ctx->disk_part_start + (fat_cluster_to_sector(ctx, current_cluster) * ctx->bpb.bytes_per_sector) + current_cluster_offset);

        current_cluster_offset += read_amount;
        file->seekpos += read_amount;
        leftover_read_count -= read_amount;
    }

    return count;
}
