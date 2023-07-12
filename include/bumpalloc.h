#pragma once
#include <types.h>

void *bump_malloc(size_t n);

// deallocate the last allocation
void bump_dealloc(size_t n);
