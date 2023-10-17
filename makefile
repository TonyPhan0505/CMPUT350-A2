# makefile for assignment
#
#  make        refreshes executable 
#  make clean  removes all object files and str
#  make dep    creates list of dependencies that can 
#              be pasted at the end
PROG := simul

# uncomment for debug mode
CCOPTS := -Wall -Wextra -O -g -std=c++17

# uncomment for release mode
#CCOPTS := -Wall -Wextra -O3 -DNDEBUG -std=c++17

# compiler and linker
CC := g++ 

# how to create .o files from .c files
%.o : %.cpp makefile
	$(CC) $(CCOPTS) -c -o $@ $<

# how to create executable from object files
# (without rpath... the program crashes before calling main on the lab
# machines)
$(PROG) : simul.o World.o World2.o Marine.o Tank.o
# Option 1: create mesa version (can be run remotely when using ssh -X ...)
#	$(CC) -o $@ $^ -Wl,-rpath,/usr/lib/x86_64-linux-gnu/mesa -lGLU -lGL -lglut
# Option 2: create local version (only runs sitting in front of lab machines)
	$(CC) -o $@ $^ -lglut -lGLEW -lGL -lGLU -lX11 -L/usr/lib/nvidia-375 -Wno-write-strings

# print dependencies
dep:
	g++ -MM *.cpp

# remove object files
clean:  
	rm -f *.o $(PROG)

# other file dependencies
# (output of make dep)
Marine.o: Marine.cpp Marine.h Unit.h World.h
simul.o: simul.cpp World.h Unit.h Marine.h Tank.h
Tank.o: Tank.cpp Tank.h Unit.h World.h
World2.o: World2.cpp World.h
World.o: World.cpp World.h Unit.h
