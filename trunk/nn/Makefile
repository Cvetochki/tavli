DEBUG=-g
CC=g++
CXXFLAGS=-O3 -march=native -mfpmath=sse -msse2 -fomit-frame-pointer -funroll-loops -Wunsafe-loop-optimizations 
#CXXFLAGS=-Wall $(DEBUG)
#LDFLAGS=-Wall $(DEBUG)
#-Wunreachable-code

ICC=icc
ICCFLAGS=-xT -ipo -O9
#ICCFLAGS=-xHost -ipo -O9
#ICCFLAGS=-pg

OBJS=nn.o CNeuralNet.o bench.o


#CC=$(ICC)
#CXX=$(ICC)
#CXXFLAGS=$(ICCFLAGS)

nn: $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o nn

clean:
	rm -f $(OBJS) nn 

nn.o: nn.cpp types.h Makefile
CNeuralNet.o: CNeuralNet.cpp types.h Makefile
bench.o: bench.cpp types.h Makefile
