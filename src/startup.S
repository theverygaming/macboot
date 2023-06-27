// floppy entry code from https://gist.github.com/kmcallister/3236565ed7eb7b45cf99
.section .entry
.global _kentry
.type _kentry, @function
_kentry:
    .short  0x4C4B              /* boot blocks signature */
    bra     start               /* entry point to bootcode */
    .short  0x4418              /* boot blocks version number */
    .short  0x00                /* used internally */
    
    // Pascal strings - 16 byte per string (.string has null terminator)
    .byte 14 
    .string "foo bar       "    /* System filename */
    .byte 14 
    .string "foo bar       "    /* Finder filename */
    .byte 14 
    .string "foo bar       "    /* debugger filename */
    .byte 14 
    .string "foo bar       "    /* debugger filename */
    .byte 14 
    .string "foo bar       "    /* name of startup screen */
    .byte 14 
    .string "foo bar       "    /* name of startup program */
    .byte 14 
    .string "foo bar       "    /* name of system scrap file */

    .short  10                  /* number of FCBs to allocate */
    .short  20                  /* number of event queue elements */
    .long   0x00004300          /* system heap size on 128K Mac */
    .long   0x00008000          /* used internally */
SysHeapSize:
    .long   0x00020000          /* system heap size on all machines */
start:
    //move.w #0x2700, %sr // supervisor set, Interrupt priority 7

    move.l 0x2a6,%a0 // SysZone
    add.l %pc@(SysHeapSize),%a0
    .short 0xa057 // _SetAppBase
    move.l 0x2a6, 0x118 // SysZone -> TheZone

    // check if there is 4K of memory at 0x10000
    move.l 0x0108, %d0      // get memory top pointer
    move.l #0xAB01, %d7
    cmpi.l #0x11000,%d0
    ble error
    move.l 0x02b2, %d0      // get memory base pointer
    move.l #0xAB02, %d7
    cmpi.l #0x10000,%d0
    bgt error

    /*
    // allocate memory for disk read
    move.l #4096, %a0
    .short 0xA11E
    move.l %a0, %d0
    move.l #0xAB03, %d7
    cmpi.l #0,%d0
    beq error
    move.l %a0, %a1
    */

    move.l #0x10000, %a1

    lea	IOParam_ioVRefNum(%pc),%a0
    move.w 0x0210, (%a0)             // BootDrive
    lea	IOParam_ioRefNum(%pc),%a0
    move.w 0x0B34, (%a0)             // BtDskRfn
    lea	IOParam_ioBuffer(%pc),%a0
    move.l %a1, (%a0)
    lea	IOParam_ioReqCount(%pc),%a0
    move.l #4096, (%a0)
    
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
