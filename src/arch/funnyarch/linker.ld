{
    "start": 16777216,
    "sections": [
        {"name": ".entry"},
        {"name": ".text"},
        {"name": ".rodata"},
        {"name": ".data"},
        {"name": ".bss"}
    ],
    "anchor_symbols": [
        {"name": "_bss_start", "anchor_section_start": ".bss"},
        {"name": "_bss_end", "anchor_section_end": ".bss"},
        {"name": "_bootloader_start", "anchor_section_start": ".entry"},
        {"name": "_bootloader_end", "anchor_section_end": ".bss"}
    ]
}
