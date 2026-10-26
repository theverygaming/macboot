// floppy entry code from https://gist.github.com/kmcallister/3236565ed7eb7b45cf99
.section .entry
.global _kentry
.type _kentry, @function
_kentry:
    // boot block header - https://developer.apple.com/library/archive/documentation/mac/pdf/Operating_System_Utilities/Start_Mgr.pdf
    .short  0x4C4B              /* boot blocks signature */
    bra     code                /* entry point to bootcode */
    .short  0x4418              /* boot blocks version number - sill have no idea what half of this means but it seems to work */
    .short  0x0000              /* reserved */
    
    // Pascal strings - 16 byte per string
    .byte 6
    .ascii "System         "    /* System filename */
    .byte 6 
    .ascii "Finder         "    /* Finder filename */
    .byte 7 
    .ascii "Macsbug        "    /* first debugger filename */
    .byte 12 
    .ascii "Disassembler   "    /* second debugger filename */
    .byte 13
    .ascii "StartUpScreen  "    /* name of startup screen */
    .byte 6
    .ascii "Finder         "    /* name of startup program */
    .byte 14
    .ascii "Clipboard File "    /* name of system scrap file */

    .short  0x000C              /* number of FCBs to allocate */
    .short  0x001E              /* number of event queue elements */
    .long   0x00004200          /* system heap size on 128K Mac */
    .long   0x00008000          /* reserved */
SysHeapSize:
    .long   0x0000C000          /* system heap size on machines with 512K or more RAM */
code:
    move.l 0x2a6,%a0 // SysZone
    add.l %pc@(SysHeapSize),%a0
    .short 0xa057 // _SetAppBase
    move.l 0x2a6, 0x118 // SysZone -> TheZone

    // check if there is enough of memory at _bootloader_start
    move.l 0x0108, %d0      // get memory top pointer
    move.l #0xAB01, %d7
    cmpi.l #_bootloader_end,%d0
    ble error
    move.l 0x02b2, %d0      // get memory base pointer
    move.l #0xAB02, %d7
    cmpi.l #_bootloader_start,%d0
    bgt error

    move.l #_bootloader_start, %a1

    // calculate amount of memory required for bootloader - result in %d1
    move.l #_bootloader_end, %d1
    move.l #_bootloader_start, %d2
    sub.l %d2, %d1

    lea	IOParam_ioVRefNum(%pc),%a0
    move.w 0x0210, (%a0)             // BootDrive
    lea	IOParam_ioRefNum(%pc),%a0
    move.w 0x0B34, (%a0)             // BtDskRfn
    lea	IOParam_ioBuffer(%pc),%a0
    move.l %a1, (%a0)
    lea	IOParam_ioReqCount(%pc),%a0
    move.l %d1, (%a0)
    
    lea	IOParam(%pc),%a0
    //.short 0xA202                    // _Read (synchronous) - for some reason apple docs are wrong here? bit 9 set does not work?
    .short 0xA002                      // _Read (synchronous)
    move.l #0xAB04, %d7
    cmpi.w  #0, %d0
    bne error

    jmp (%a1,(entry - _kentry))

entry:
    bsr _kcppentry

    move.l #0xE926, %d0 // error code
    .short 0xA9C9      // SysError

error:
    move.l %d7, %d0 // error code
    .short 0xA9C9      // SysError
    jmp error

IOParam:
    .long	0	// qLink - internally used
	.short	0	// qType - internally used
	.short  0	// ioTrap - internally used
	.long	0	// ioCmdAddr - internally used
	.long	0	// ioCompletion
	.short	0	// ioResult
	.long	0	// ioNamePtr
IOParam_ioVRefNum:
	.short	0	// ioVRefNum
IOParam_ioRefNum:
	.short	0	// ioRefNum
	.byte	0	// ioVersNum - not used
	.byte	1	// ioPermssn - allow only reads (0x1)
	.long	0	// ioMisc - not used
IOParam_ioBuffer:
	.long	0   // ioBuffer
IOParam_ioReqCount:
	.long	0   // ioReqCount
IOParam_ioActCount:
	.long	0	// ioActCount
	.short	1	// ioPosMode - IO position mode: from start
IOParam_ioPosOffset:
	.long	0   // ioPosOffset
