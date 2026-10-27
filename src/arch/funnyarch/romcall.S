.section .text
.global romcall
romcall:
    strpi rsp, r8, #-4
    strpi rsp, r9, #-4

    int #128

    ldri r9, rsp, #4
    ldri r8, rsp, #4
    mov rip, lr
