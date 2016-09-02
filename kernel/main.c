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

#include <asm/unistd.h>		/* _exit() .. */
#include <unistd.h>		/* write()    */
#include <sched.h>

#define CHK_VALUE 0x12345678

extern void time_init();
extern void app_start();

/* initial kernel stack (now moved to .data section) pointer
 * Note that stack -must not- reside in .bss section. Since clear_bss()
 * is a C function, it uses stack for its function return address
 * and local variables. If the stack is in .bss, clear_bss() erases
 * its own variables and the program hangs!
 *
 * This is a bug in the last embed_example!
 */
char * const stack_start = (char *)&init_task_union.stack[STACK_SIZE/sizeof(long)];

void clear_bss() {
	extern char __bss_start[],_end[];
	int i, len = _end - __bss_start;
	
	for (i=0; i<len; i++)
		__bss_start[i] = 0;
}

/*
 * The idle thread. There's no useful work to be
 * done, so just try to conserve power and have a
 * low exit latency (ie sit in a loop waiting for
 * somebody to say that they'd like to reschedule)
 * 
 * Note that cpu_relax() is a barrier by itself.
 * need_resched is forced reloaded from memory in 
 * every loop.
 */
void cpu_idle (void)
{
	while (1) {
		while (!need_resched) {
			cpu_relax();
		}
		schedule();
	}
}

/* Programming NOTE:

   Don't use global/static uninitialzed variables before clear_bss()
*/
void start_kernel() {
	const char kernel_msg[] = __FILE__": mini kernel started.\n";
	const char bsserr[] = __FILE__": error initializing .bss section\n";

	static int bss_var;			/* at .bss section */

	write(STDOUT_FILENO, kernel_msg, sizeof(kernel_msg)); 

	bss_var = CHK_VALUE;	
	clear_bss();

	/* check if BSS section is initialized */
	if (bss_var != 0) {
		write(STDERR_FILENO, bsserr, sizeof(bsserr));
		_exit(1);
	}

	local_irq_disable();

	/* 
	 * Interrupts are still disabled. Do necessary setups, then
	 * enable them.
       	 * 
	 * setup_arch(); 
	 */

        /*
         * Set up the scheduler prior starting any interrupts (such as the
         * timer interrupt).
	 */
	sched_init(); 

	/*
	 * Disable preemption - early bootup scheduling is extremely
	 * fragile until we cpu_idle() for the first time.
	 */
	preempt_disable();

	/* build_all_zonelists();
	 * trap_init();
	 * init_IRQ();
	 * softirq_init(); 
	 */
	time_init();
	/* console_init(); */

	local_irq_enable();

	/* mem_init(); */

	/* start user threads defined in apps.c */
	app_start();

	preempt_enable();
	cpu_idle();
}
