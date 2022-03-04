
all: example
.PHONY: clean

%.o: %.c
	cc -c $<

example: example.o libtest.o
	cc -o $@ $^

clean:
	rm example *.o
