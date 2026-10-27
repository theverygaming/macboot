#pragma once
#include <types.h>

#define LITTLEBOOT_HAS_STDINT
#include <proto.h>

void boot();
void enter_kernel(struct littleboot_kernel_header *kheader);
