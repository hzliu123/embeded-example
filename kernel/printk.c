#include <asm/unistd.h>
#include <unistd.h>		/* write() */
#include <kernel.h>

extern unsigned long jiffies;

#define xBUF_SIZE 128

/* printk is designed to be reentrant
 * no use of static or global variables 
 */
int __printk(const char *fmt, ...) {
	va_list args;
	char xbuf[xBUF_SIZE], *str = xbuf;
	int i, spcleft = xBUF_SIZE;

	i = scnprintf(str, spcleft, "[% 8lu] ", jiffies);
	str += i;
	spcleft -= i;

	va_start(args, fmt);
	i += vscnprintf(str, spcleft, fmt, args);
	va_end(args);
	
	if (i > 0) 
		i = write(STDOUT_FILENO, xbuf, i);
	return i;
}
