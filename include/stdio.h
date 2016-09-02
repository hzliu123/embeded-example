#ifndef _STDIO_H_
#define _STDIO_H_
#include <stddef.h>
#include <stdarg.h>		/* for va_list */

extern int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
extern int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);
extern int snprintf(char * buf, size_t size, const char *fmt, ...);
extern int scnprintf(char * buf, size_t size, const char *fmt, ...);
extern int vsprintf(char *buf, const char *fmt, va_list args);
extern int sprintf(char * buf, const char *fmt, ...);

#endif
