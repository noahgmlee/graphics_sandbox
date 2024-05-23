CXXFLAGS = -g -Wall -std=c++17 -Ishared

SHARED_SRCS = $(wildcard shared/*.cpp)
SHARED_OBJS = $(patsubst shared/%.cpp, bin/%.o, $(SHARED_SRCS))
ROLLER_SRCS = $(wildcard Rollercoaster/*.cpp)
ROLLER_OBJS = $(patsubst Rollercoaster/%.cpp, bin/%.o, $(ROLLER_SRCS))

all: shared buildMVP buildRC

buildRC: $(SHARED_OBJS) $(ROLLER_OBJS) bin/glad.o
	$(CXX) $(CXXFLAGS) -g $^ -o bin/roller -lglfw

bin/%.o: Rollercoaster/%.cpp
	$(CXX) $(CXXFLAGS) -g -c $< -o $@ -Wno-writable-strings

buildMVP: $(SHARED_OBJS) bin/glad.o
	$(CXX) $(CXXFLAGS) -g MVP/MVP.cpp $^ -o bin/MVP -lglfw

bin/%.o: shared/%.cpp
	$(CXX) $(CXXFLAGS) -g -c $< -o $@ -Wno-writable-strings

bin/glad.o:
	gcc -g -c glad/glad.c -o bin/glad.o

runMVP:
	./bin/MVP

runRC:
	./bin/roller

clean:
	rm -rf bin/*