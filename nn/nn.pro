######################################################################
# Automatically generated by qmake (2.01a) Wed Aug 3 23:17:03 2011
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -march=native -mfpmath=sse -msse4.2 -fomit-frame-pointer -funroll-loops -Wunsafe-loop-optimizations
# Input
HEADERS += CNeuralNet.h types.h
SOURCES += bench.cpp CNeuralNet.cpp nn.cpp
