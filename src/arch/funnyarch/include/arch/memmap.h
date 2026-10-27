#pragma once
#include <types.h>
extern uint32_t mem_reclaimable1_start;
extern uint32_t mem_reclaimable1_size;
extern uint32_t mem_reclaimable2_start;
extern uint32_t mem_reclaimable2_size;
extern uint32_t mem_bumpalloc_start;
extern uint32_t mem_bumpalloc_size;
extern uint32_t mem_usable_start;
extern uint32_t mem_usable_size;

bool funnyarch_mem_init(uint32_t membase, uint32_t membytes);
