ENTRY(_kentry)

PHYS_START = 0x13000;

SECTIONS {
	. = PHYS_START;
	_kernel_start = ABSOLUTE(.);

	.text : ALIGN(2)  {
        KEEP(*(.entry))
		KEEP(*(.text))
	}

	.data : ALIGN(2) {
		KEEP(*(.data))
	}

	.rodata : ALIGN(2) {
		KEEP(*(.rodata))
	}

	.bss : ALIGN(2) {
		_bss_start = ABSOLUTE(.);
		KEEP(*(.bss))
		_bss_end = ABSOLUTE(.);
	}

	_kernel_end = ABSOLUTE(.);
}
_kernel_size = _kernel_end - _kernel_start;
