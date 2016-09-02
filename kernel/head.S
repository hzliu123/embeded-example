/* setup stack pointer and call a C routine */


.text

	.globl startup_32
	
startup_32:
	movl stack_start,%esp	# setup stack pointer

	movl $4, %eax		# write() system call
	movl $1, %ebx		# fd = stdout
	leal msg, %ecx		# %ecx ---> msg
	movl msg_len, %edx	# %edx = count
	int  $0x80		# execute write() system call

	call start_kernel

.data

msg:
	.ascii __FILE__": initialize stack pointer and call start_kernel()\n"
msg_len:
	.long . - msg
