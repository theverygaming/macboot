#include <boot/config.h>
#include <fat/fat.h>

static int fgetc(struct fat_driver_state *fs, struct fat_file_handle *ff) {
    unsigned char r;
    if (fat_read(fs, ff, &r, sizeof(r)) != sizeof(r)) {
        return -1;
    }
    return r;
}

// str will be null-terminated (str_len includes null termination) - if value does not fit in str it will be cut off
bool get_cfg_value(struct fat_driver_state *fs, struct fat_file_handle *ff, const char *key, char *str, size_t str_len) {
    fat_seek(fs, ff, 0, FAT_SEEK_BEGIN);
    // search for the key
    while (true) {
        const char *key_inc = key;
        int s2 = fgetc(fs, ff);
        while ((*key_inc != '\0') && (*key_inc == ((char)s2)) && (s2 >= 0) && (((char)s2) != '\n')) {
            key_inc++;
            s2 = fgetc(fs, ff);
        }
        if ((((char)s2) == '=') && (*key_inc == '\0')) { // matches
            break;
        }
        if (s2 < 0) { // read error
            return false;
        }
        if (((char)s2) == '\n') { // newline
            continue;
        }
        // no match
        // goto next newline
        while (true) {
            int c = fgetc(fs, ff);
            if (c < 0 || c == '\n') {
                break;
            }
        }
    }
    // write value
    size_t counter = 0;
    while (true) {
        int c = fgetc(fs, ff);
        if ((c < 0) || (((char)c) == '\n')) {
            break;
        }
        if (counter >= (str_len - 1)) {
            break;
        }
        str[counter] = (char)c;
        counter++;
    }
    str[counter] = '\0';
    return true;
}
