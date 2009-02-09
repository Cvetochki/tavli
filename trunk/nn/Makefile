DEBUG=-g
CC=g++
#CXXFLAGS=-O3 -march=pentium4 -mfpmath=sse -msse2 -fomit-frame-pointer -funroll-loops -Wunsafe-loop-optimizations 
CXXFLAGS=-Wall $(DEBUG)
LDFLAGS=-Wall $(DEBUG)
#-Wunreachable-code

ICC=icpc
#ICCFLAGS=-xT -ipo -O9
#ICCFLAGS=-xHost -ipo -O9
ICCFLAGS=-pg

OBJS=nn.o CNeuralNet.o bench.o

nn: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o nn

#CC=$(ICC)
#CXX=$(ICC)
#CXXFLAGS=$(ICCFLAGS)

nn.icc: $(OBJS)

clean:
	rm -f $(OBJS) nn nn.icc

nn.o: nn.cpp types.h Makefile
CNeuralNet.o: CNeuralNet.cpp types.h Makefile
bench.o: bench.cpp types.h Makefile
