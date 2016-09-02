/* process.c 
 *
 * don't have memory allocator yet. cannot runtime-allocate task 
 * structures. so compile-time declared task_union array is used
 */
#include <sched.h>
#include <kernel.h>

int last_pid = 0;		/* last pid number allocated */
int nr_threads = 0;

unsigned int need_resched = 0;
struct task_struct *current;

/* task_union array for new task's PCB allocation. 
 * init_task does not use this array. It has its own task_union
 *  declaration 
 */
#define MAX_THREADS sizeof(int)*8
static union task_union tu_array[MAX_THREADS];
static unsigned int tu_alloc_map = 0;	/* tu_array status, 0=free */

union task_union init_task_union = { .task = INIT_TASK(init_task_union.task) };
struct task_struct * const init_task = &init_task_union.task;

/* 
 * -- need protection, not race safe --
 * ffs() search in tu_alloc_map for a free slot.
 * if found, a task_union is return, 
 * otherwise return NULL
 */
union task_union *alloc_task_union() {
	int i;
	union task_union *ret = NULL;

	i = ffs(~tu_alloc_map);	/* i = 1 to 32 */
	if (i > 0) {
		__set_bit(i-1, (unsigned long *)&tu_alloc_map);
		ret = &tu_array[i-1];
	}
	return ret;
}

void free_task_union(union task_union *tu) {
	int i;

	i = tu - &tu_array[0];
	__clear_bit(i, (unsigned long *)&tu_alloc_map);
}

/* 
 * No zombie process. recycle everything!
 * Note that schedule() still use(runs on) the -deallocated- 
 * task_union(stack). To avoid it being use by other newly created
 * tasks, preemption must be disabled until schedule() finished!
 */ 
void do_exit(int nouse) {
	union task_union *current_tu;

	current_tu = container_of(current, union task_union, task);

	/* preempt_disable() don't need to pair with 
	   preempt_enable(), this task is going to exit anyway */
	preempt_disable();

	nr_threads--;
	current->state = TASK_EXITING;
	list_del(&current->tasks);
	free_task_union(current_tu);
	schedule();

	/* should not go here */	
	printk("BUG: Dead process becomes alive!\n");
	for (;;)
		cpu_relax();
}

#define asmlinkage __attribute__((regparm(0)))
/* 
 * kernel_thread_helper to catch exiting process and
 * call do_exit() to recycle memory allocated 
 */
static asmlinkage void kernel_thread_helper(int (*fn)(void *), void *arg) {
	int exit_code;

	/* new task has an initially disabled preempt_count */
	preempt_enable();
	exit_code = fn(arg);
	do_exit(exit_code);
}

/*
 * Create a kernel thread
 */
int kernel_thread(int (*fn)(void *), void * arg, char *name) {
	union task_union *n;
	struct task_struct *nt;
	unsigned long *esp;
	int ret = -1;

	preempt_disable();
	if (nr_threads >= MAX_THREADS) goto out;
	n = alloc_task_union();
	if (!n) goto out;

	/* setup new thread's task_struct */
	nt = &n->task;
	nt->state = TASK_RUNNING;
	nt->time_slice = HZ;

	/* 
	 * NOTE: Since current is also swapped during context_switch(), new 
	 * task must have a initially disabled preempt_count to avoid being 
	 * preempted prematurely. Timer interrupt calls schedule() if 
	 * current->preempt_count == 0 
	 */
	nt->preempt_count = 0;	/* CHANGE TO 1 AFTER SYNC. CODE IS READY */
	nt->pid = ++last_pid;
	snprintf(nt->comm, TASK_COMM_LEN, name);
	list_add_tail(&nt->tasks, &init_task->tasks);

	/* 
	 * setup new thread's stack and processor state:
	 * push fn and arg to the stack for kernel_thread_helper
	 * stack content: ret. addr(no use), fn addr, fn arg
	 */
	esp = &n->stack[STACK_SIZE/sizeof(long)];
	esp -= 3;
	*(esp+1) = (unsigned long)fn;
	*(esp+2) = (unsigned long)arg;
	nt->thread.esp = (unsigned long)esp;
	nt->thread.eip = (unsigned long)kernel_thread_helper; 

	nr_threads++;
	ret = nt->pid;
out:
	preempt_enable();
	return ret;	
}

static inline struct task_struct *context_switch(struct task_struct *prev,
	       struct task_struct *next) {

	/*
	 * switch_mm: switch MMU related registers.
	 * ie. process page table directory 
	 */

	current = next;
	switch_to(prev, next, prev);

	return prev;
}	

/*
 * main scheduler function: a simple round-robin scheduler.
 * no separate runqueue, tasks are searched/moved around on
 * init_task tasks list. 
 */
void schedule() {
	struct task_struct *prev, *next;

	/* -- YOU WRITE CODE HERE --
	 * preemption should be disabled to avoid race condition if schedule() 
	 * is called from timer interrupt. you will need to reset need_resched
	 * flag, adjust the order of current task in the task list, find next
	 * task on the task list, and call context_switch(prev, next) to switch
	 * tasks. Note that TASK_EXITING task is not on the task list.
	 */

	if (prev != next)
		context_switch(prev, next);

	return;
}

/* setup scheduler data structure */
void sched_init() {
	current = init_task;
}
