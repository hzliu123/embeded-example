/* apps.c
 * User's code should be limited in this file.
 * After kernel starts, it calls app_start() to start 
 * user applications
 */

#include <sched.h>

/* 
 * A queue with 2 pointers, one pointer for read by the consumer 
 * and one pointer for write by the producer.
 *
 * Since process synchronization mechanism (mutex/wait queue..) is not 
 * available yet, the performance of produce/consume can vary greatly depending
 * on the way it is implemented.
 *
 * When the queue is full/empty, producer/consumer can:
 * 1) just busy looping until the queue is not full/empty.
 * 2) call schedule() while looping, in the hope that consumer/producer gets
 *    the cputime and read/write the queue. 
 */
#define QLEN 10
static int queue[QLEN];
static int ridx, widx;

#define queue_empty() (ridx == widx)
#define queue_full() (ridx-widx == 1 || widx-ridx == QLEN-1)

/* -- PRONE TO RACE CONDITION, YOU ADD SYNCHRONIZATION CODE BELOW -- */  
static int producer(void *nouse) {
	int enqueue_count = 0, enqueue_sum = 0;

	printk("Producer thread started.\n");

	while (1) {
try_queue:
		if (queue_full()) {
			if (enqueue_count > 0) {
				printk("%d values queued, sum = %d\n", 
					enqueue_count, enqueue_sum);
				enqueue_sum = enqueue_count = 0;
			}
			cpu_relax();
			/* remove line below after timer interrupt is up */
			schedule();
			continue;
		}
		
		enqueue_count++;
		enqueue_sum += queue[widx++] = jiffies * 0xfb & 0xff;
		if (widx >= QLEN) widx = 0;
		goto try_queue;
	}

	return 0;
}


/* -- PRONE TO RACE CONDITION, YOU ADD SYNCHRONIZATION CODE BELOW -- */  
static int consumer(void *nouse) {
	int dequeue_count = 0, dequeue_sum = 0;

	printk("Consumer thread started.\n");

	while (1) {
try_dequeue:
		if (queue_empty()) {
			if (dequeue_count > 0) {
				printk("%d values dequeued, sum = %d\n", 
					dequeue_count, dequeue_sum);
				dequeue_sum = dequeue_count = 0;
			}
			cpu_relax();
			/* remove line below after interrupt is up */
			schedule();
			continue;
		}

		dequeue_count++;
		dequeue_sum += queue[ridx++];
		if (ridx >= QLEN) ridx = 0;
		goto try_dequeue;
	}

	return 0;
}

/* User application entry point */
void app_start() {
	ridx = widx = 0;

	kernel_thread(producer, NULL, "producer");
	kernel_thread(consumer, NULL, "consumer");
}
