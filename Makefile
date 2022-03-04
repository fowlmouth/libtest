
CFLAGS := -Iinclude

PREFIX := /usr/local

all: example
.PHONY: clean install

libtest.a: libtest.o
	ar rcs $@ $<

example: example.o libtest.a
	cc -o $@ $^

install: libtest.a
	mkdir -p "$(PREFIX)/lib" "$(PREFIX)/include"
	install libtest.a "$(PREFIX)/lib/"
	install include/*.h "$(PREFIX)/include/"

clean:
	rm -f example *.o *.a
