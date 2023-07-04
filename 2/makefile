all: ClubAgeStats Position Team

ClubAgeStats: ClubAgeStats.cpp ClubAgeStats.h
	g++ -std=c++17 ClubAgeStats.cpp -o ClubAgeStats.out

Position: Position.cpp Position.h
	g++ -std=c++17 Position.cpp -o Position.out

Team: Team.cpp Team.h
	g++ -std=c++17 Team.cpp -o Team.out

.PHONY: clean

clean:
	rm GenreCounter Position Team