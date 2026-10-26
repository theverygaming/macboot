#pragma once
#include <types.h>

#define MACBOOT_HAS_STDINT
#include <proto.h>

void boot();
void enter_kernel(struct macboot_kernel_header *kheader);
