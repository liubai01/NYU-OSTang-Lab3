CC=g++
CFLAGS=-g -pedantic -pthread # -Wall -Werror -Wextra
SOURCEDIR = src
BUILDDIR = build
OBJECTS = utils task taskqueue bufferpool

.PHONY: all
all: nyuenc

nyuenc: nyuenc.o $(addsuffix .o, $(OBJECTS))
	$(CC) -o $@ $(addprefix $(BUILDDIR)/,$^) $(CFLAGS)

nyuenc.o: nyuenc.cpp
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)

%.o: $(SOURCEDIR)/%.cpp $(SOURCEDIR)/%.hpp
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)

.PHONY: clean pacakge grade profile
clean:
	rm -f *.o nyuenc
	rm -f $(BUILDDIR)/*.o
	rm -f nyuenc-yx2412.tar.xz
	rm -f *.txt
	rm -f grading/nyuenc-yx2412.tar.xz
	rm -f grading/nyuenc
	rm -f grading/nyuenc.cpp
	rm -f grading/Makefile
	rm -rf grading/myoutputs
	rm -rf grading/src/*.cpp
	rm -rf grading/src/*.hpp
	rm -rf grading/build
	rm output.png
	cd ..

	

package:
	tar cJf nyuenc-yx2412.tar.xz Makefile build/DONOTREMOVEDIR src nyuenc.cpp

grade:
	tar cJf grading/nyuenc-yx2412.tar.xz Makefile build/DONOTREMOVEDIR src nyuenc.cpp
	cd grading && bash ./autograder.sh && cd ..

time:
	time --portability ./nyuenc grading/inputs/5.in > /dev/null
	time --portability ./nyuenc -j5 grading/inputs/5.in > /dev/null

profile:
	sudo perf record -g -- ./nyuenc -j3 grading/inputs/5.in > /dev/null
	sudo perf script | c++filt | gprof2dot -f perf | dot -Tpng -o output.png
	rm -f perf.data
	rm -f perf.data.old