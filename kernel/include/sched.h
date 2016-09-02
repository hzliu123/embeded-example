#ifndef _SCHED_H_
#define _SCHED_H_
#include <asm/unistd.h>
#include <signal.h>
#include <kernel.h>		/* HZ */
#include <list.h>

/* 
 * Note: 
 * 1) For Newly-borned process, we switch to kernel_thread_helper
 *    instead of label "1"
 * 2) "memory" is added to clobber list because values of any variables 
 *    cached in registers might be modified after a task gives up its
 *    CPU time. switch_to() also acts as a memory barrier.
 */
#define switch_to(prev, next, last)					\
do {									\
	/*								\
	 * Context-switching clobbers all registers, so we clobber	\
	 * them explicitly, via unused output variables.		\
	 */								\
	unsigned long ebx, ecx, edx, esi, edi;				\
									\
	asm volatile("pushfl\n\t"		/* save    flags */	\
		     "pushl %%ebp\n\t"		/* save    EBP   */	\
		     "movl %%esp,%0\n\t"	/* save    ESP   */ 	\
		     "movl %8,%%esp\n\t"	/* restore ESP   */ 	\
		     "movl $1f,%1\n\t"		/* save    EIP   */	\
		     "jmp *%9\n\t"		/* restore EIP   */	\
		     "1:\t"						\
		     "popl %%ebp\n\t"		/* restore EBP   */	\
		     "popfl\n"			/* restore flags */	\
									\
		     /* output parameters */				\
		     : "=m" (prev->thread.esp), 			\
		       "=m" (prev->thread.eip),				\
		       "=a" (last),					\
									\
		     /* clobbered output registers: */			\
		       "=b" (ebx), "=c" (ecx), "=d" (edx),		\
		       "=S" (esi), "=D" (edi)				\
									\
		     /* input parameters: */				\
		     : "m" (next->thread.esp),				\
		       "m" (next->thread.eip),				\
		       "a" (prev)					\
									\
		     /* clobber list: untrust any variables in regs */	\
		     : "memory");					\
} while (0)


/* task state */
#define TASK_RUNNING	0
#define TASK_WAITING	1
#define TASK_STOPPED	2
#define TASK_EXITING	3

#define TASK_COMM_LEN 	16

/* 
 * processor state:
 * only esp, eip saved here.
 * the rest are on stack
 */
struct thread_struct {
	unsigned long esp;
	unsigned long eip;
};


#define INIT_TASK(tsk) \
{						\
	.state	= TASK_RUNNING,			\
	.time_slice = HZ,			\
	.preempt_count = 0,			\
	.pid = 0,				\
	.comm = "swapper",			\
	.tasks = LIST_HEAD_INIT(tsk.tasks), 	\
}

/*
 * process control block:
 */
struct task_struct {
	volatile long state;		/* TASK_RUNNING, ..etc. */
	unsigned int time_slice;	/* time quantum */
	int preempt_count;		/* >0 => cannot be context-switched */
	int pid;			/* unique process id */
	char comm[TASK_COMM_LEN];	/* name of the process */
	struct list_head tasks;		/* linked list of all tasks */
	struct thread_struct thread;	/* processor state save */
};


#define STACK_SIZE 4096
/*
 * process control block and its stack
 * share the same space
 */
union task_union {
	struct task_struct task;
	unsigned long stack[STACK_SIZE/sizeof(long)];
};

extern int nr_threads;
/* timer interrupt checks this flag and cause a reschedule if it's true */
extern unsigned int need_resched;
/* pointer to current process's task_struct */
extern struct task_struct *current;
/* init_task represents the initial context that eventually run cpu_idle() */
extern union task_union init_task_union;
extern struct task_struct * const init_task;
extern volatile unsigned long jiffies;

/* note: sigprocmask() is itself a barrier() */
static inline void local_irq_save(sigset_t *oset) {
	sigset_t set;
	sigfillset(&set);
	sigprocmask(SIG_SETMASK, &set, oset);
}

static inline void local_irq_restore(sigset_t *oset) {
	sigprocmask(SIG_SETMASK, oset, NULL);
}

static inline void local_irq_disable() {
	sigset_t set;
	sigfillset(&set);
	sigprocmask(SIG_SETMASK, &set, NULL);
}

static inline void local_irq_enable() {
	sigset_t set;
	sigemptyset(&set);
	sigprocmask(SIG_SETMASK, &set, NULL);
}

/* REP NOP (PAUSE) is a good thing to insert into busy-wait loops. */
#define cpu_relax() __asm__ __volatile__("rep;nop": : :"memory");
#define barrier() __asm__ __volatile__("": : :"memory")

#define preempt_disable()		\
do { 					\
	current->preempt_count++;	\
	barrier();			\
} while (0)

#define preempt_enable_no_resched()	\
do { 					\
	barrier();			\
	current->preempt_count--;	\
} while (0)

#define preempt_check_resched()		\
do {					\
	if (need_resched && current->preempt_count == 0) \
		schedule();		\
} while (0)

#define preempt_enable()		\
do {					\
	preempt_enable_no_resched();	\
	barrier();			\
	preempt_check_resched();	\
} while (0)

extern void sched_init();
extern void schedule();
int kernel_thread(int (*fn)(void *), void * arg, char *name);

#endif
