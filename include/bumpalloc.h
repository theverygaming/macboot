#pragma once
#include <types.h>

extern uint8_t *bumpalloc_begin;
extern uint8_t *bumpalloc_end;
void bumpalloc_init();

void *bump_malloc(size_t n);

// deallocate the last allocation
void bump_dealloc(size_t n);
