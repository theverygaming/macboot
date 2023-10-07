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

    // https://developer.apple.com/library/archive/documentation/mac/pdf/ImagingWithQuickDraw.pdf
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

    // https://developer.apple.com/library/archive/documentation/mac/pdf/Imaging_With_QuickDraw/Color_QuickDraw.pdf
    struct PixMap {
        void *baseAddr;
        int16_t rowBytes;
        struct qd::Rect bounds;
        int16_t pmVerion;
        int16_t packType;
        uint32_t packSize;
        int32_t hRes;
        int32_t vRes;
        int16_t pixelType;
        int16_t pixelSize;
        int16_t cmpCount;
        int16_t cmpSize;
        int32_t planeBytes;
        void *pmTable;
        int32_t pmReserved;
    };

    // https://developer.apple.com/library/archive/documentation/mac/pdf/Imaging_With_QuickDraw/Graphics_Devices.pdf
    struct GDevice {
        int16_t gdRefNum;
        int16_t gdID;
        int16_t gdType;
        void *gdITable;
        int16_t gdResPref;
        void *gdSearchProc;
        void *gdCompProc;
        int16_t gdFlags;
        struct PixMap **gdPMap;
        int32_t gdRefCon;
        int32_t GdNextGD;
        struct qd::Rect gdRect;
        int32_t gdMode;
        int16_t gdCCBytes;
        int16_t gdCCDepth;
        void *gdCCXdata;
        void *gdCCXMask;
        int32_t gdReserved;
    };

    void InitGraf(rom::GrafPort *port);
}
