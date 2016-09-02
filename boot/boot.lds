/* 
   Simulated boot loader by Hao-Ran Liu

   This linker script is used when making Image.elf, which includes head.S,
   boot.c and piggy.o.

   Every output section has two addresses.  The first is the "VMA", or 
   virtual memory address.  This is the address the section will have when
   the output file is run.  The second is the "LMA", or load memory address.
   This is the address at which the section will be loaded. In most cases 
   the two addresses will be the same.  An example of when they might be 
   different is when a data section is loaded into ROM, and then copied 
   into RAM when the program starts up (this technique is often used to 
   initialize global variables in a ROM based system).  In this case the 
   ROM address would be the LMA, and the RAM address would be the VMA.
   You can change LMA by using the 'AT' keyword.It makes it easy to build
   a ROM image.

   When making runtime image from ELF file, objcopy consults LMA addresses
   in each section. It reorders sections by their LMA addresses in ascending
   order and copy those sections in the arranged order to the output file, 
   starting from the first LMA address. If there is a memory gap between 
   two consecutive sections, it fill the gap with zeros.

   In this example, with the help of 'AT' keyword, .data section for the
   runtime image can be moved to the space between .rodata and .kdata 
   sections. This largely reduces the size of image file. Without 'AT' 
   keyword, objcopy would produce a image file of 5MBytes.

   Image.elf memory map:

   section
   name  	file offset	LMA address	VMA address

   .text	0x00001000	0x00100000	0x00100000
   .rodata	<-------------- follow .text ------------>
   .data	<------ follow .rodata --->	0x005F8000
   .bss		N/A		x               <-follow .data->
   .kdata  	<follow .data>  0x00110000      0x00110000

*/

ENTRY(startup_32)
SECTIONS {
	. = 0x00100000;
	.text : {
		*(.text)
	}

	.rodata : {
		*(.rodata)
	}
	_sdata_rom = .;
	
	. = 0x00110000;
	.kdata : {			/* for piggy.o */
		*(.kdata)	
	}

	. = 0x005F8000;			/* This is runtime address (VMA) */
	_sdata = .;
	.data : AT(_sdata_rom) {	/* but store after .rodata */
		*(.data)
	}
	
	_sbss = .;
	.bss : {
		*(.bss)
	}
	_end = .;
}
