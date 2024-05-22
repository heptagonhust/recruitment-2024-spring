CXXFLAGS=-Og -g -fsanitize=address -Wall -Wextra -Wshadow  -pipe

all: main

main: baseline.o main.o solution.o
	g++ $(CXXFLAGS) -o $@ $^

%.o: %.cc
	g++ $(CXXFLAGS) -c -o $@ $^

clean:
	rm -f *.o main
