/*
	Simulated boot loader by Hao-Ran Liu
 
	This is an example demonstrating building an embedded platform.
	My goal is to reveal the mystery behind the scenes with an 
        extremely-simplified example, so most code will be in C.
	
	The demo code runs in Linux user level for two reasons.
	1) Tracing the code will be easier with the help of user-level
           debugger.
        2) GCC can only produce 32-bit code, but x86 starts in 16-bit mode.
           There must be some assembly codes to switch CPU into x86 
           protected mode and fill necessary segment registers.  
           I don't like too many machine-dependent assembly codes in the
           example. 

	bootloader memory map:

	0x00100000		<--- start of ROM area --->
				Boot loader (.text+.rodata)	64KB
	0x00110000
				piggy.o (kernel) (.kdata)	1MB-64KB
	0x001FFFFF		<--- end of ROM area ----->
	0x00200000		<--- start of DRAM area -->
				 ..kernel start address..

	0x005F8000		<-bootldr data,bss start-->

				 ..bootldr stack start....
	0x005FFFFF		<--- end of DRAM area ---->
*/

#include <asm/unistd.h>		/* Linux kernel header for system call */
#include <unistd.h>

#define CHK_VALUE 0x12345678

/* 
   The following symbols are defined in linker script 'boot.lds'
   
   _sdata_rom: start address of .data on ROM
   _sdata: start address of .data on RAM
   _sbss: start address of .bss on RAM (also end address of .data on RAM)
   _end: end address of whole image
*/
extern char _sdata_rom[], _sdata[], _sbss[], _end[];

/* called by head.S to initialize .bss section */
void clear_bss() {
	const char bsserr[] = __FILE__": error initializing .bss section\n";

	int i, len = _end - _sbss;
	static int chk_value;	/* a test variable in .bss */

	chk_value = CHK_VALUE;
	for (i=0; i<len; i++) 
		_sbss[i] = 0;
 
	if (chk_value != 0) {
		write(STDERR_FILENO, bsserr, sizeof(bsserr));
		_exit(1);
	}
}

/* called by head.S to copy .data from ROM to RAM */
void init_data() {
	const char dataerr[] = __FILE__": error initializing .data section\n";

	static int chk_value = CHK_VALUE; /* a test variable in .data */
	int i, len = _sbss - _sdata;

	for (i=0; i<len; i++) 
		_sdata[i] = _sdata_rom[i];

	if (chk_value != CHK_VALUE) {
		write(STDERR_FILENO, dataerr, sizeof(dataerr));
		_exit(1);
	}
}

/* DIFFERENCE BETWEEN THIS LOADER AND REAL LOADER:

   On a real machine, these jobs must be done before startup_32 is called:
   *1*. initialize CPU's DRAM-control registers
   *2*. copy .data section from ROM to RAM and initialize .bss section 
        RAM; also, setup of the stack register is needed.

   The only difference here is: no DRAM init here.

   PROGRAMMING NOTE:

   1) bss and .data sections are initialized by head.S before calling 
      start_boot, so the use of global or static variables in start_boot()
      is safe.

   2) To display a message to the console, only system calls are allowed!
      printf() or other libc functions are not available.
*/

/* start/end addr. of kernel on ROM */
extern char _skdata[], _ekdata[];
/* kernel start address on RAM */
const unsigned long kernel_start = 0x00200000;

void start_boot() {
	int i, copylen;
	const char bootmsg[] = __FILE__": start_boot() start\n";
	const char bootcopy[] = __FILE__": copy vmkernel.bin from ROM to RAM\n";
	const char bootjump[] = __FILE__": pass control to the kernel\n----------\n";

        write(STDOUT_FILENO, bootmsg, sizeof(bootmsg));	

	/* copy kernel to RAM */
	write(STDOUT_FILENO, bootcopy, sizeof(bootcopy));
	copylen = _ekdata - _skdata;
	for (i=0; i < copylen; i++) 
		*(char *)(kernel_start+i) = _skdata[i];

	/* jump to kernel */
	write(STDOUT_FILENO, bootjump, sizeof(bootjump));
	((void (*)())kernel_start)();
}

