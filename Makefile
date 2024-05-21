CXXFLAGS = -g -Wall -std=c++17 -Ishared

SHARED_SRCS = $(wildcard shared/*.cpp)
SHARED_OBJS := $(patsubst shared/%.cpp, bin/%.o, $(SHARED_SRCS))


all: shared buildMVP

buildMVP: $(SHARED_OBJS) bin/glad.o
	$(CXX) $(CXXFLAGS) -g MVP/MVP.cpp $^ -o bin/MVP -lglfw

bin/%.o: shared/%.cpp
	$(CXX) $(CXXFLAGS) -g -c $< -o $@ -Wno-writable-strings

bin/glad.o:
	gcc -g -c glad/glad.c -o bin/glad.o

runMVP:
	./bin/MVP

clean:
	rm -rf bin/*