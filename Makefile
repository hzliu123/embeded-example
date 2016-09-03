CC 	= /usr/bin/gcc
LD 	= /usr/bin/ld
OBJCOPY = /usr/bin/objcopy

TOPDIR := $(shell /bin/pwd)
HPATH   = $(TOPDIR)/include
SYSTEM  = $(TOPDIR)/vmkernel

CFLAGS  = -Wall -ffreestanding -g -fno-dwarf2-cfi-asm -I$(HPATH)
LDFLAGS =
SUBDIRS = boot kernel

ifeq ($(shell uname -m),x86_64)
	CFLAGS += -m32
	LDFLAGS += -melf_i386 --oformat=elf32-i386
else
	LDFLAGS += --oformat=elf32-i386
endif

export CC LD OBJCOPY TOPDIR HPATH CFLAGS LDFLAGS SYSTEM

.PHONY: clean subdirs $(SUBDIRS)

all: subdirs

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

boot: kernel

clean:
	for dir in $(SUBDIRS); do \
	   $(MAKE) -C $$dir clean; \
	done
