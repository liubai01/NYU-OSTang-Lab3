CC=g++
CFLAGS=-g -pedantic -Wall -Werror -Wextra
SOURCEDIR = src
BUILDDIR = build

.PHONY: all
all: nyuenc

nyuenc: nyuenc.o
	$(CC) -o $@ $(BUILDDIR)/$< $(CFLAGS)

nyuenc.o: $(SOURCEDIR)/nyuenc.c
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)
.PHONY: clean
clean:
	rm -f *.o nyuenc
	rm -f $(BUILDDIR)/*.o
