#pragma once
#include <types.h>

// str will be null-terminated (str_len includes null termination) - if value does not fit in str it will be cut off
// using seek_pos get_cfg_value can be called multiple times until it returns false. The value pointed to by seek_pos should be initialized to 0.
bool get_cfg_value(struct fat_driver_state *fs, struct fat_file_handle *ff, const char *key, char *str, size_t str_len, size_t *seek_pos = nullptr);
