#include <screen.h>
#include <arch/rom/globals.h>
#include <arch/rom/quickdraw.h>

static rom::GrafPort qd_gp;

void screen::init() {
    // figured this out thanks to https://emile.sourceforge.net/
    rom::InitGraf(&qd_gp);
    struct rom::GDevice *gd = *((struct rom::GDevice **)rom::MainDevice);

    if ((gd == nullptr) || (gd->gdPMap == nullptr) || ((*gd->gdPMap)->baseAddr == nullptr) || (gd == (struct rom::GDevice *)0xAAAAAAAA) ||
        ((qd_gp.screenBits.bounds.right - qd_gp.screenBits.bounds.left) != ((*gd->gdPMap)->bounds.right - (*gd->gdPMap)->bounds.left)) ||
        ((qd_gp.screenBits.bounds.bottom - qd_gp.screenBits.bounds.top) != ((*gd->gdPMap)->bounds.bottom - (*gd->gdPMap)->bounds.top))) {
        fb.base = qd_gp.screenBits.baseAddr;
        fb.width = qd_gp.screenBits.bounds.right - qd_gp.screenBits.bounds.left;
        fb.height = qd_gp.screenBits.bounds.bottom - qd_gp.screenBits.bounds.top;
        fb.pitch = qd_gp.screenBits.rowBytes & 0x3FFF; // upper two bits are flags

        fb.bpp = 1;
        fb.indexed = false;
        fb.bit_be = true;
    } else {
        fb.base = (*gd->gdPMap)->baseAddr;
        fb.width = (*gd->gdPMap)->bounds.right - (*gd->gdPMap)->bounds.left;
        fb.height = (*gd->gdPMap)->bounds.bottom - (*gd->gdPMap)->bounds.top;
        fb.pitch = (*gd->gdPMap)->rowBytes & 0x3FFF; // upper two bits are flags
        fb.bit_be = true;

        if ((((*gd->gdPMap)->rowBytes >> 15) & 0x1) == 0) { // turns out this is a BitMap instead of a PixMap
            // lets guess it's gonna be 1bpp
            fb.bpp = 1;
            fb.indexed = false;
            return;
        }
        fb.bpp = (*gd->gdPMap)->pixelSize;

        fb.indexed = (*gd->gdPMap)->pixelType == 0;
    }
}
