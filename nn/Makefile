CC = g++
CXXFLAGS=-O3 -march=pentium4 -mfpmath=sse -msse2 -fomit-frame-pointer -funroll-loops -Wunsafe-loop-optimizations 
#CXXFLAGS=-g
#-Wunreachable-code

ICC=icc
ICCFLAGS=-xT -ipo -O9

OBJS=nn.o CNeuralNet.o bench.o

nn: $(OBJS)

#CC=$(ICC)
#CXX=$(ICC)
#CXXFLAGS=$(ICCFLAGS)

nn.icc: $(OBJS)

clean:
	rm -f $(OBJS) nn nn.icc

nn.o: nn.cpp types.h Makefile
CNeuralNet.o: CNeuralNet.cpp types.h Makefile
bench.o: bench.cpp types.h Makefile
