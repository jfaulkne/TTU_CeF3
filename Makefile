CXXFLAGS=-Wall # -pedantic -ansi
ROOT_LIB:=`root-config --libs --glibs`
ROOT_FLAGS:=`root-config --cflags --ldflags`
ROOT_INCLUDE:=`root-config --incdir`

DEPS= interface/setOutputTree.h interface/setInputTree.h
DEPS_OBJ= lib/setOutputTree.o lib/setInputTree.o

CC = g++
CFLAGS = -Wall

lib/%.o: src/%.cc $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< $(ROOT_LIB) $(ROOT_FLAGS)

all: analyzeCeF3fibers.exe

analyzeCeF3fibers.exe: bin/analyzeCeF3fibers.cpp $(DEPS_OBJ)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LIB) $(ROOT_FLAGS)

clean:
	rm -f lib/*.o
	rm -f lib/*.d
	rm -f *.exe
