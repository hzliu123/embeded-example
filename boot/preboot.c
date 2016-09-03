/*

	preboot loads Image (bootloader + kernel) into our simulated
        ROM area.

	In real world, bootloader and kernel are already on the ROM area.
        Here, in the simulated environment, the Image is put into 
        simulated-ROM area by preboot. We don't rely on ELF loader, since
        we want to load text, bss, data section from ROM to DRAM by 
        ourselves.

	System memory map:

	0x00000000		<--- preboot start ------>
	0x000FFFFF		<--- preboot end -------->
	0x00100000		<--- start of ROM area --->
	0x001FFFFF		<--- end of ROM area ----->
	0x00200000		<--- start of DRAM area -->
	0x005FFFFF		<--- end of DRAM area ---->
*/

#include <unistd.h>
#include <asm/unistd.h>
#include <asm/fcntl.h>

#define READSIZE 1024
#define IMG_FILENAME "Image" 
const unsigned long brkptr = 0x00600000;
const unsigned long boot_start = 0x00100000;

/* NOTE: _start() is the entry point, ie. no one calls _start.
   To end _start() peacefully, you have to use _exit(),
   not the keyword "return"; 
*/

void _start() {
	char pbmsg[] = __FILE__": _start() start...\n";
	char pbcopy[] = __FILE__": copy Image to ROM\n";
	char pbjump[] = __FILE__": pass control to boot loader\n----------\n";
	char brkerr[] = __FILE__": failed to increase data segment space.\n";
	char image_err[] = __FILE__": failed to stat/open \"Image\" file.\n";
	char readerr[] = __FILE__": read \"Image\" failed.\n";

	int imgfd, i, byte_read;
	struct stat statbuf;
	char *ptr = (char *)boot_start;

        write(STDOUT_FILENO, pbmsg, sizeof(pbmsg));	

	/* get more space to copy Image to ROM */ 
	if (brk(brkptr) != brkptr) {
		write(STDERR_FILENO, brkerr, sizeof(brkerr));
		_exit(1);
	}

	/* open Image and get file size */
	if (stat(IMG_FILENAME, &statbuf) != 0) {
		write(STDERR_FILENO, image_err, sizeof(image_err));
		_exit(1); 
	}
	if ((imgfd = open("Image", O_RDONLY, 0)) == -1) {
		write(STDERR_FILENO, image_err, sizeof(image_err));
		_exit(1);
	}

	/* copy Image contents to ROM */
	write(STDOUT_FILENO, pbcopy, sizeof(pbcopy));
	i = 0;
	while (1) {
		byte_read = read(imgfd, ptr+i, READSIZE);
		if (byte_read < 0) break;
		i += byte_read;
		if (byte_read < READSIZE) break;
	}

	/* check if the number of bytes we copied equals to the file size */
	if (statbuf.st_size != i) {
		write(STDERR_FILENO, readerr, sizeof(readerr));
		_exit(1);
	}

	/* jump to boot loader */
	write(STDOUT_FILENO, pbjump, sizeof(pbjump));
	((void (*)())boot_start)();

}
