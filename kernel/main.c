/*
    This is the kernel.

    vmkernel.bin is the image starting from offset 0x00200000.
    It contains .text, .rodata and .data sections. 
    So all we have to do here is initialize stack pointer
    and .bss sections.

    kernel memory map:

	0x00100000		<--- start of ROM area --->
				Boot loader (.text+.rodata)	64KB
	0x00110000
				piggy.o (kernel) (.kdata)	1MB-64KB
	0x001FFFFF		<--- end of ROM area ----->
	0x00200000		<--- start of DRAM area -->
				 ..kernel start address..

	0x005FFFFF		<--- end of DRAM area ---->
*/

#include <unistd.h>
#include <asm/unistd.h>

#define STACK_SIZE 8192
#define CHK_VALUE 0x12345678

char stack_space[STACK_SIZE];		/* located in .bss section */
char *stack_start = &stack_space[STACK_SIZE];

void clear_bss() {
	extern char __bss_start[],_end[];
	int i, len = _end - __bss_start;
	
	for (i=0; i<len; i++)
		__bss_start[i] = 0;
}

/* Programming NOTE:

   Don't use global/static uninitialzed variables before clear_bss()
*/

start_kernel() {
	char kernel_msg[] = __FILE__": mini kernel started.\n";
	char bsserr[] = __FILE__": error initializing .bss section\n";

	static int bss_var;			/* at .bss section */

	write(STDOUT_FILENO, kernel_msg, sizeof(kernel_msg)); 

	bss_var = CHK_VALUE;	
	clear_bss();

	/* check if BSS section is initialized */
	if (bss_var != 0) {
		write(STDERR_FILENO, bsserr, sizeof(bsserr));
		_exit(1);
	}

	for(;;);
}

