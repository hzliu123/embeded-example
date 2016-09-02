# pre-boot loader address map
# NOTE: Linux 2.6 does not allow user program to use the first 64k 
#       virtual address, .text cannot start at address 0 anymore
PREBOOT_TEXT = 0x00010000
PREBOOT_DATA = 0x00012000
PREBOOT_LDFLAGS = -Ttext $(PREBOOT_TEXT) -Tdata $(PREBOOT_DATA) 

IMAGE_LDFLAGS = -T boot.lds

all: Image preboot

# generic pattern rules to compile a .c to a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.S
	$(CC) $(CFLAGS) -c $<

# kernel is embedded inside piggy.o
# kernel must be in binary format since ELF loader is not available
piggy.o: $(SYSTEM)
	$(OBJCOPY) -O binary $(SYSTEM) vmkernel.bin
	$(LD) -o $@ -r --format binary --oformat elf32-i386 vmkernel.bin -T piggy.lds
	rm -f vmkernel.bin

# Image is bootloader + piggy.o
Image: head.o boot.o piggy.o
	$(LD) $(IMAGE_LDFLAGS) -o $@.elf $^
	$(OBJCOPY) -O binary $@.elf $@

# run preboot to load Image, preboot is in ELF format
preboot: preboot.o
	$(LD) $(PREBOOT_LDFLAGS) -o $@ $<
 
clean:
	rm -f preboot preboot.o head.o boot.o piggy.o Image.elf Image

