#include <rom/quickdraw.h>

extern "C" void _InitGraf(void *ptr);

void rom::InitGraf(rom::GrafPort *port) {
    _InitGraf(&port->thePort);
}
