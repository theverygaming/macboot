ENTRY(_kentry)

PHYS_START = 0x10000;

MEMORY
{
	ram (rwx) : org = PHYS_START, l = 4096
}

SECTIONS {
	. = PHYS_START;

	.text : ALIGN(2)  {
        KEEP(*(.entry))
		KEEP(*(.text))
	}>ram

	.rodata : ALIGN(2) {
		KEEP(*(.rodata))
	}>ram

	.data : ALIGN(2) {
		KEEP(*(.data))
	}>ram

	.bss : ALIGN(2) {
		_bss_start = ABSOLUTE(.);
		KEEP(*(.bss))
		_bss_end = ABSOLUTE(.);
	}>ram
}
