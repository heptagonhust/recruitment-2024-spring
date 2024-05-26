CXXFLAGS=-Og -g -fsanitize=address -Wall -Wextra -Wshadow  -pipe

all: main

main: baseline.o main.o solution.o
	g++ $(CXXFLAGS) -o $@ $^

baseline.o: baseline.cc
	g++ -O0 -Wall -Wextra -Wshadow -pipe -c -o $@ $^

%.o: %.cc
	g++ $(CXXFLAGS) -c -o $@ $^

clean:
	rm -f *.o main
