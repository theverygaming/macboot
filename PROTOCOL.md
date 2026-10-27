# littleboot protocol

## about this boot protocol

Since this is _very very_ similar to the Limine boot protocol, go read [this](https://github.com/Limine-Bootloader/limine-protocol/blob/3a0526b700e356f0eac1b71a77697b3fd1c707a3/PROTOCOL.md#features) section of the Limine protocol docs if you want to know the basics.

Take a look at the example kernel in the `example` directory to get started.

## State of CPU on kernel entry
### m68k
`sp` will point to a stack in bootloader reclaimable memory, this stack is at least 1024 bytes in size. There is a return value pushed to the stack. If you return from your kernel the bootloader will go into an infinite loop.

`sr` will have the supervisor bit set and interrupt priority will be 7

### funnyarch
`rsp` will point to a stack in bootloader reclaimable memory, this stack is at least 1024 bytes in size. There is a return in `lr`. If you return from your kernel the bootloader will go into an infinite loop.
