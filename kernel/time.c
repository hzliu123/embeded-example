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
	jiffies++;
	if (jiffies % (TIMER_PRINTK_RATE*HZ) == 0)
		printk("%d threads running\n", nr_threads); 

	if (current->time_slice > 0) 
		if (--current->time_slice == 0) {
			need_resched = 1;
			current->time_slice = HZ;
		}

	/* 
	 * equivalent to Linux ret_from_intr:
	 * if need_resched and preemptable, run schedule()
	 */
	if (need_resched && current->preempt_count == 0)
		schedule();
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
 */
void time_init() {
	const unsigned long tick_usec = 1000000 / HZ;
	const unsigned long usec_per_sec = 1000000;
	struct sigaction act;
	struct itimerval value;

	/* xtime: wall time init */

	/* 
	 * setup_irq()
	 * hook signal handler as timer interrupt handler
	 */
	act.sa_handler = timer_interrupt;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_NODEFER;
	sigaction(SIGALRM, &act, NULL);

	/* 
	 * set_pit_timer() - timer chip programming
	 * setup Linux interval timer
	 */
	value.it_interval.tv_sec = tick_usec / usec_per_sec;
	value.it_interval.tv_usec = tick_usec % usec_per_sec;
	value.it_value = value.it_interval;
	setitimer(ITIMER_REAL, &value, NULL);

	printk("Timer initialized.\n");	
}
