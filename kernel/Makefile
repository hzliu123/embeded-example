LDFLAGS += -T vmkernel.lds

all: $(SYSTEM)

%.o: %.S
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(SYSTEM): head.o main.o
	$(LD) $(LDFLAGS) -o $@ $^
clean:
	rm -f head.o main.o $(SYSTEM)

