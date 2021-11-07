CC=g++
CFLAGS=-g -pedantic -pthread # -Wall -Werror -Wextra
SOURCEDIR = src
BUILDDIR = build
OBJECTS = utils task taskqueue bufferpool

.PHONY: all
all: nyuenc

nyuenc: nyuenc.o $(addsuffix .o, $(OBJECTS))
	$(CC) -o $@ $(addprefix $(BUILDDIR)/,$^) $(CFLAGS)

nyuenc.o: $(SOURCEDIR)/nyuenc.cpp
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)

%.o: $(SOURCEDIR)/%.cpp $(SOURCEDIR)/%.hpp
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o nyuenc
	rm -f $(BUILDDIR)/*.o
