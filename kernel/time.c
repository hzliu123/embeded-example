#include <asm/unistd.h>
#include <unistd.h>
#include <signal.h>		/* sigaction */
#include <sys/time.h>		/* setitimer argument */
#include <sched.h>

/* every TIMER_PRINTK_RATE seconds, timer interrupt printk(), 
 * indicating that kernel is still alive
 */
#define TIMER_PRINTK_RATE 10

volatile unsigned long jiffies = 0;


void timer_interrupt(int num) {

	/* -- YOU WRITE CODE HERE -- 
	 * jiffies and current process timeslice update,
	 * if timeslice == 0, set the need_resched flag
	 * if need_resched and preemptable, run schedule()
	 */
}

/* 
 * NOTE on Linux's signal processing
 * The signal handler installed by sigaction() will be masked after it starts
 * execution, and unmasked after it returns. Linux kernel setups a signal stack
 * frame on the user stack when a signal is triggered, and if it returns, the
 * return code on its stack frame calls sigreturn() to notify the kernel about
 * its end. sigreturn() unwinds signal stack frame on user stack and restores
 * process's signal mask.
 *
 * In our case, SIGALRM signal handler does not return when it does a context
 * switch. The signal stack frame is not unwinded and sigreturn() is not 
 * called. This leaves SIGALRM masked (timer interrupt blocked) after SIGALRM
 * handler (timer interrupt) switches context. We use SA_NODEFER sa_flags to
 * solve the problem.
 *
 * 2011/06 -- Use SIGVTALRM instead so that no additional signal occurs when
 *            the program is stopped by GDB.
 */
void time_init() {

	/* -- YOU WRITE CODE HERE -- 
	 * 1) setup_irq()
	 *    hook signal handler as timer interrupt handler
	 * 2) set_pit_timer() - timer chip programming
	 *    setup Linux interval timer
	 */
}
