# Macboot protocol

## about this boot protocol

Since this is _very very_ similar to the Limine boot protocol, go read [this](https://github.com/limine-bootloader/limine/blob/trunk/PROTOCOL.md#features) section of the Limine protocol docs if you want to know the basics.

Take a look at the example kernel in the `example` directory to get started.

## State of CPU on kernel entry
`sp` will point to a stack in bootloader reclaimable memory, this stack is at least 1024 bytes in size. There is a return value pushed to the stack. If you return from your kernel the bootloader will go into an infinite loop

`sr` will have the supervisor bit set and interrupt priority will be 7
