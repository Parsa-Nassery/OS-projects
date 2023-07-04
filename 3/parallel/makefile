all: parallel


parallel: parallel.cpp
	g++ -pthread parallel.cpp -o ImageFilters.out

.PHONY: clean
clean:
	rm parallel