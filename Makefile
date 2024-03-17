.PHONY: all clean

CXX       = g++-12
CXXFLAGS += -I. --std=c++14 -g -O3 -march=native -mavx2 $(shell pkg-config --cflags sdl2) -fext-numeric-literals
LDFLAGS  += $(shell pkg-config --libs sdl2)
LIBS     += -lpthread

CXX_SOURCES = main.cpp $(shell find src -type f -name "*.cpp")
CXX_OBJS=$(subst .cpp,.o,$(CXX_SOURCES))

CXXFLAGS += -DSIMD

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(CXX_OBJS)
	$(CXX) $(CXX_OBJS) $(LDFLAGS) $(LIBS) -o mandelbrot.exe

clean:
	rm -rf *.o *.exe
	find . -type f -name  "*.o" -delete
	
