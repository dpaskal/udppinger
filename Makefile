FLAGS = -std=c++14 -Wall -Wextra -pedantic -g

all: client server

client: client.cpp
	g++ client.cpp -o client $(FLAGS)

server: server.cpp
	g++ server.cpp -o server $(FLAGS)

clean:
	rm -f client server