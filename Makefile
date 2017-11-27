################################################################################
# Makefile for dagSW software
# Gabor T. Marth 2015
# University of Utah
################################################################################

################################################################################
# To run (examples)
################################################################################

# optimized
# make CFLAGS="-O9 -static"

# optimized + static
# make CFLAGS="-O9 -static"

# optimized + static + 32bit
# make CFLAGS="-O9 -static -m32"

# to profile
# make CFLAGS="-O9 -static -pg"

################################################################################
# Variables
################################################################################

# Compiler
CC=g++

# Compiler flags
CFLAGS=

################################################################################
# Program builds
################################################################################
all: sw gsw

sw: sw.o Class-Alignment.o Class-Node.o
	$(CC) $(CFLAGS) sw.o Class-Alignment.o -lboost_regex -lpthread -o sw

gsw: gsw.o Class-GraphAlignment.o Class-Node.o Class-Traceback.o Class-Pileup.o Class-Graph.o ArrayUtil.o 
	$(CC) $(CFLAGS) gsw.o Class-Node.o Class-GraphAlignment.o Class-Traceback.o Class-Pileup.o Class-Graph.p ArrayUtil.o -lboost_regex -lpthread -o gsw

################################################################################
# Compilation
################################################################################

# Main program compilations
sw.o: sw.cpp
	$(CC) $(CFLAGS) -c sw.cpp -o sw.o

gsw.o: gsw.cpp
	$(CC) $(CFLAGS) -c gsw.cpp -o gsw.o

ArrayUtil.o: ArrayUtil.cpp
	$(CC) $(CFLAGS) -c ArrayUtil.cpp -o ArrayUtil.o

Class-Graph.o: Class-Graph.cpp
	$(CC) $(CFLAGS) -c Class-Graph.cpp -o Class-Graph.p

Class-Traceback.o: Class-Traceback.cpp
	$(CC) $(CFLAGS) -c Class-Traceback.cpp -o Class-Traceback.o

Class-Pileup.o: Class-Pileup.cpp
	$(CC) $(CFLAGS) -c Class-Pileup.cpp -o Class-Pileup.o

Class-Alignment.o: Class-Alignment.cpp
	$(CC) $(CFLAGS) -c Class-Alignment.cpp -o Class-Alignment.o

Class-GraphAlignment.o: Class-GraphAlignment.cpp
	$(CC) $(CFLAGS) -c Class-GraphAlignment.cpp -o Class-GraphAlignment.o

Class-Node.o: Class-Node.cpp
	$(CC) $(CFLAGS) -c Class-Node.cpp -o Class-Node.o

################################################################################
# Archival
################################################################################
archive:	sw.cpp Class-Alignment.cpp Class-Alignment.h Class-Pileup.h \
	gsw.cpp Class-GraphAlignment.cpp Class-GraphAlignment.h Class-Traceback.h \
	 Class-Node.cpp  Class-Node.h Class-Graph.cpp Class-Graph.h Makefile 
	tar cvf GSW.tar sw.cpp Class-Alignment.cpp Class-Alignment.h \
	gsw.cpp Class-GraphAlignment.cpp Class-GraphAlignment.h \
	Class-Node.cpp  Class-Node.h ArrayUtil.o Makefile; \
	gzip GSW.tar

################################################################################
# Installation
################################################################################

################################################################################
# House-keeping
################################################################################
clean:
	rm -rf *.o sw gsw

