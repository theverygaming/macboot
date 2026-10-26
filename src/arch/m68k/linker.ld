ENTRY(_kentry)

PHYS_START = 0x10000;
PHYS_LEN = 12288;

MEMORY
{
	ram (rwx) : org = PHYS_START, l = PHYS_LEN
}

SECTIONS {
	. = PHYS_START;
	_bootloader_start = ABSOLUTE(.);

	.text : ALIGN(2)  {
        KEEP(*(.entry))
		KEEP(*(.text))
	}>ram

	.data : ALIGN(2) {
		KEEP(*(.data))
	}>ram

	.rodata : ALIGN(2) {
		KEEP(*(.rodata))
	}>ram

	.bss : ALIGN(2) {
		_bss_start = ABSOLUTE(.);
		KEEP(*(.bss))
		_bss_end = ABSOLUTE(.);
	}>ram

	_malloc_begin = ABSOLUTE(.);

	. = PHYS_START + PHYS_LEN;

	_bootloader_end = ABSOLUTE(.);
}
