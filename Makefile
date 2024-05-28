CXXFLAGS = -g -Wall -std=c++17 -Ishared

SHARED_SRCS = $(wildcard shared/*.cpp)
SHARED_OBJS = $(patsubst shared/%.cpp, bin/%.o, $(SHARED_SRCS))
ROLLER_SRCS = $(wildcard Rollercoaster/*.cpp)
ROLLER_OBJS = $(patsubst Rollercoaster/%.cpp, Rollercoaster/bin/%.o, $(ROLLER_SRCS))
MVP_SRCS = $(wildcard MVP/*.cpp)
MVP_OBJS = $(patsubst MVP/%.cpp, MVP/bin/%.o, $(MVP_SRCS))

all: shared buildR buildM buildRC buildMVP

buildMVP: $(SHARED_OBJS) $(MVP_OBJS) bin/glad.o
	$(CXX) $(CXXFLAGS) -g $^ -o bin/MVP -lglfw

MVP/bin/%.o: MVP/%.cpp
	$(CXX) $(CXXFLAGS) -g -c $< -o $@ -Wno-writable-strings

buildRC: $(SHARED_OBJS) $(ROLLER_OBJS) bin/glad.o
	$(CXX) $(CXXFLAGS) -g $^ -o bin/roller -lglfw

Rollercoaster/bin/%.o: Rollercoaster/%.cpp
	$(CXX) $(CXXFLAGS) -g -c $< -o $@ -Wno-writable-strings

buildM: $(SHARED_OBJS) bin/glad.o
	$(CXX) $(CXXFLAGS) -g M/M.cpp $^ -o bin/M -lglfw

buildR: $(SHARED_OBJS) bin/glad.o
	$(CXX) $(CXXFLAGS) -g R/R.cpp $^ -o bin/R -lglfw

bin/%.o: shared/%.cpp
	$(CXX) $(CXXFLAGS) -g -c $< -o $@ -Wno-writable-strings

bin/glad.o:
	gcc -g -c glad/glad.c -o bin/glad.o

runR:
	./bin/R

runM:
	./bin/M

runRC:
	./bin/roller

runMVP:
	./bin/MVP

clean:
	rm -rf bin/*
	rm -rf MVP/bin/*
	rm -rf Rollercoaster/bin/*