.section .text

.global _kernelentry
.type _kernelentry, @function
_kernelentry:
move.l %sp@(8),%a0 // kernel entry
move.l %sp@(4),%a1 // new stack pointer
move.w #0x2700, %sr
move.l %a1, %sp
jsr (%a0)
.loop:
nop
jmp .loop
