CC 	= /usr/bin/gcc
LD 	= /usr/bin/ld
OBJCOPY = /usr/bin/objcopy
AR	= /usr/bin/ar

TOPDIR := $(shell /bin/pwd)
HPATH   = $(TOPDIR)/include
LIBPATH = $(TOPDIR)/lib
SYSTEM  = $(TOPDIR)/vmkernel

CFLAGS  = -Wall -ffreestanding -g -fno-dwarf2-cfi-asm -I$(HPATH)
SUBDIRS = lib boot kernel

export CC LD OBJCOPY AR TOPDIR HPATH LIBPATH CFLAGS SYSTEM

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
