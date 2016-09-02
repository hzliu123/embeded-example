/* setup stack pointer and call a C routine */


.text

	.globl startup_32
	
startup_32:
	movl $0x00600000,%esp	# setup stack pointer
	call clear_bss		# zero-init bss section on RAM
	call init_data		# copy data section from ROM to RAM

	movl $4, %eax		# write() system call
	movl $1, %ebx		# fd = stdout
	leal msg, %ecx		# %ecx ---> msg
	movl msg_len, %edx	# %edx = count
	int  $0x80		# execute write() system call

	call start_boot

.data

msg:
	.ascii __FILE__": init %esp, copy .data to RAM, zero-init .bss, call start_boot()\n"
msg_len:
	.long . - msg
