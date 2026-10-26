.section .text

/*
calling convention:
we can trash a0, a1, d0 and d1
values returned in d0
*/

/*
rom trashes:
a0, a1, d0, d1, d2
*/

.macro rcinit
linkw %fp,#0
move.l %d2,-(%sp)
.endm

.macro rcexit
move.l (%sp)+,%d2
unlk %fp
rts
.endm

.global _Read
.type _Read, @function
_Read:
    rcinit
    move.l %fp@(8),%a0
    move.l #0x0, %d0
    .short 0xA002 // _Read(%a0)
    rcexit

.global _SysError
.type _SysError, @function
_SysError:
    rcinit
    move.w %fp@(8),%d0
    .short 0xA9C9 // _SysError(%d0)
    rcexit

.global _InitGraf
.type _InitGraf, @function
_InitGraf:
    rcinit
    move.l %fp@(8),%d0
    move.l %d0,-(%sp)
    .short 0xA86E // _InitGraf
    add.l #4, %sp
    rcexit
