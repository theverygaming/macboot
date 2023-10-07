static void sound() {
    // resources: linux/arch/m68k/include/asm/mac_via.h, linux/arch/m68k/mac/via.c
    /*
    // uint32_t vrega = rom::VIA + 7680;
    uint32_t vregb = rom::VIA;
    uint8_t *vregb_p = (uint8_t *)vregb;
    *vregb_p &= ~(1 << 7); // enable sound
    const volatile uint32_t &pwmbuf = *(volatile const uint32_t *const)(0x0B0A);
    memset((void *)pwmbuf, 0x55, 100);
    */
}

void experiments() {
    sound();
    
}
