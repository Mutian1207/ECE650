all: ringmaster player

ringmaster: ringmaster.o helper.cpp 
	g++ -std=c++11 -g -o $@ $< helper.cpp
player: player.o helper.cpp
	g++ -std=c++11 -g -o $@ $< helper.cpp
%.o: %.cpp
	g++ -std=c++11 -g -c $<
ringmaster.o:helper.hpp
player.o:helper.hpp

.PHONY:
	clean
clean:
	rm -rf *.o ringmaster player *.cpp~ *.hpp~ *#
