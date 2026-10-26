#pragma once
#include <types.h>

namespace memmap {
    // called in a loop until it returns false. ctx initialized to nullptr (used for keeping track of iteration state internally)
    bool iter(paddr_t *base, size_t *size, int *type, void **ctx);
}
