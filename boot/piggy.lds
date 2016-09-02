/* the output object here is piggy.o, which contains the vmkernel.bin
   in its data section. 

   SECTIONS commands tells the linker how to map input sections into 
   output sections. "." is location counter, which always refer to a
   location in an output section. * is a wildcard to match the filenames 
   of all input objects. 

   We defines 2 symbols _skdata and _ekdata, and their values are the 
   memory addresses of start and end of .kdata respectively.
   *(.data)

   *NOTE* the 2 symbols don't occupy any space in the data 
          section. They are just symbols in the symbol table.

	  Kernel binary are embedded separately in its own section
	  .kdata because we put it in its own space on ROM.
	  This allows better kernel binary to be updated separately
	  without update of boot loader.
*/

SECTIONS
{
  .kdata : { 
	_skdata = .; 
	*(.data) 
	_ekdata = .; 
	}
}
