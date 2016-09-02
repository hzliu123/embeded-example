LDFLAGS	= -T vmkernel.lds -L $(LIBPATH) -lprintf
PWD	:= $(shell pwd)
CFLAGS	+= -I$(PWD)/include 
SYSOBJS := head.o main.o printk.o time.o process.o apps.o

all: $(SYSTEM)

%.o: %.S
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(SYSTEM): $(SYSOBJS)
	$(LD) -o $@ $^ $(LDFLAGS) 
clean:
	rm -f $(SYSOBJS) $(SYSTEM)

