all: server player

server: server.cpp helper.cpp
	g++ -g -o $@ server.cpp helper.cpp 
player: player.cpp helper.cpp 
	g++ -g -o player player.cpp helper.cpp 

.PHONY:
	clean
clean:
	rm -rf *.o server player *~ *#
