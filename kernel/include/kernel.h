#ifndef _KERNEL_H_
#define _KERNEL_H_
#include <stdarg.h>		/* va_list */

#ifndef NULL
#define NULL 0
#endif

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/**
 * container_of - cast a member of a structure out to the containing structure
 *
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})

#define ADDR (*(volatile long *) addr)

/**
 * ffs - find first bit set
 * @x: the word to search
 *
 * This is defined the same way as
 * the libc and compiler builtin ffs routines, therefore
 * differs in spirit from the above ffz (man ffs).
 */
static inline int ffs(int x)
{
	int r;

	__asm__("bsfl %1,%0\n\t"
		"jnz 1f\n\t"
		"movl $-1,%0\n"
		"1:" : "=r" (r) : "rm" (x));
	return r+1;
}

/**
 * __set_bit - Set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * Unlike set_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static inline void __set_bit(int nr, volatile unsigned long * addr)
{
	__asm__(
		"btsl %1,%0"
		:"=m" (ADDR)
		:"Ir" (nr));
}

static inline void __clear_bit(int nr, volatile unsigned long * addr)
{
	__asm__ __volatile__(
		"btrl %1,%0"
		:"=m" (ADDR)
		:"Ir" (nr));
}

#define HZ 1		/* frequency of timer interrupt, must >= 1 */

#define printk(FMT, ...) __printk("%s: "FMT, __func__, ##__VA_ARGS__)
extern int __printk(const char *fmt, ...);
extern int snprintf(char * buf, size_t size, const char *fmt, ...);
extern int scnprintf(char * buf, size_t size, const char *fmt, ...);
extern int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);

#endif
