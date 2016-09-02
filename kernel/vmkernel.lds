/* .bss section is a section with no contents. However, an area in memory
   should be set aside and zero-inited for it 

   __bss_start and _end will be used in main.c to initialize .bss section
*/

ENTRY(startup_32)
SECTIONS {
	. = 0x00200000;
	.text : {
		*(.text)
	}

	.rodata : {
		*(.rodata)
	}

	.data : {
		*(.data)
	}

	.bss : {
		__bss_start = .;
		*(.bss)
	}
	_end = .;

	/DISCARD/ : {
		*(.eh_frame)
	}
}
