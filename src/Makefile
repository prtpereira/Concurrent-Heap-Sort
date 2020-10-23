CXX = g++
CXXFLAGS = -std=c++14 -O3 -lpthread

EXEC = heap
FILES = heap.cpp

heap: $(FILES)
	${CXX} $(FILES) -o $(EXEC) ${CXXFLAGS} ${INCLUDES}

run:
	./$(EXEC)

clean:
	rm $(EXEC)

all: heap run

allc: clean heap run
