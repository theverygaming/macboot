#pragma once
#include <types.h>

extern "C" void *memset(void *ptr, int value, size_t n);
extern "C" void *memcpy(void *dst, const void *src, size_t n);

extern "C" char *strncpy(char *destination, const char *source, size_t num);
