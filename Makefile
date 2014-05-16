CC = gcc
CFLAGS = -Wall -std=c99 -fPIC -shared
CXX = g++
CXXFLAGS = -Isrc/Analyzer -Wall -std=c++11 -MMD -MP
SRCS = $(wildcard src/Analyzer/**/*.cpp src/Analyzer/*.cpp)
OBJS = $(patsubst src/%.cpp, obj/%.o, $(SRCS))

analyzer: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o bin/analyzer

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(OBJS:%.o=%.d)


lib:
	gcc $(CFLAGS) src/SharedLib/lib_filesystem.c -o bin/lib_filesystem.so -ldl

all: analyzer lib
	
run:
	bin/analyzer

clean:
	rm -r bin/* obj/*

doc:
	doxygen doc/doxygen.config

graph:
	dot -Tpdf -Gmargin=0 out/graph/graph.gv -o out/graph/graph.pdf

.PHONY: all lib clean doc
