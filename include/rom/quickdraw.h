#pragma once
#include <types.h>

namespace rom {
    namespace qd {
        struct Pattern {
            uint8_t pat[8];
        };

        struct Rect {
            int16_t top;
            int16_t left;
            int16_t bottom;
            int16_t right;
        };

        struct BitMap {
            void *baseAddr;
            uint16_t rowBytes;
            struct Rect bounds;
        };

        struct Point {
            uint16_t v;
            uint16_t h;
        };

        struct Cursor {
            int16_t data[16];
            int16_t mask[16];
            struct Point hotSpot;
        };
    }

    struct GrafPort {
        uint8_t _internal[76];
        int32_t randSeed;
        struct qd::BitMap screenBits;
        struct qd::Cursor arrow;
        struct qd::Pattern dkGray;
        struct qd::Pattern ltGray;
        struct qd::Pattern gray;
        struct qd::Pattern black;
        struct qd::Pattern white;
        void *thePort;
    };

    void InitGraf(rom::GrafPort *port);
}
