LIBS = libprintf.a

all: $(LIBS)

%.o: %.S
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

libprintf.a: printf.o
	$(AR) rs $@ $^

clean:
	rm -f printf.o $(LIBS)

