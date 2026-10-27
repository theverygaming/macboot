.section .text
.global _kernelentry
_kernelentry:
    // r0 = new stack pointer
    // r1 = kernel entry address
    mov rsp, r0
    mov lr, .Lloop.low
    movh lr, .Lloop.high
    mov rip, r1
.Lloop:
    nop
    rjmp .Lloop
