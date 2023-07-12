#include <rom/error.h>

extern "C" void _SysError(uint16_t code);

void rom::SysError(uint16_t code) {
    _SysError(code);
}
