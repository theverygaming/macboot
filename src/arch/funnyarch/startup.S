.section .entry
_entry:
    // magic bytes
    .byte 0x52
    .byte 0x52
    .byte 0x3A
    .byte 0x33
    // r0 = memory base, r1 = amount of memory
    // set up stack at top of memory
    add rsp, r0, r1
    // align rsp
    mov r2, #0x3
    not r2, r2
    and rsp, rsp, r2
    sub rsp, rsp, #4
    // save memory base and amount of memory (r0 and r1 will be trashed by the ROM)
    strpi rsp, r0, #-4
    strpi rsp, r1, #-4

    // save memory base in r10
    mov r10, r0

    // allow unaligned access
    mov r2, #0
    mtsr pcst, r2

    // figure out how many _extra_ sectors to read to load all bootloader code. Amount of sectors in r11
    mov r0, _bootloader_start.low
    movh r0, _bootloader_start.high
    mov r1, _bootloader_end.low
    movh r1, _bootloader_end.high
    sub r11, r1, r0
    // align up to 512
    add r11, #511
    mov r0, #511
    not r0, r0
    and r11, r11, r0
    // divide by 512 to obtain sector count
    shr r11, r11, #9

    // add 512 to memory base (bootsector)
    add r10, #512

    // FIXME: before we load data from the disk we should check if we have enough space (we need some for our stack too!!)

    // load extra sectors
    mov r12, #1 // sector counter
.Lread_sectors:
    // have we read enough sectors?
    cmp r12, r11
    ifgteq rjmp .Lread_sectors_end
    // read sector
    mov r0, #0 // function = read sector from disk
    mov r1, r12 // sector number
    mov r2, r10 // memory address
    int #128
    // increment counters
    add r12, #1
    add r10, #512
    rjmp .Lread_sectors
.Lread_sectors_end:

    // restore memory base and amount of memory
    ldri r1, rsp, #4
    ldri r0, rsp, #4

    // enter C++
    rjal _cppentry

.Lloop:
    rjmp .Lloop
