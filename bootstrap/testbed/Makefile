all: client server

client: client.c ../msgpipe.c
	gcc -Wall -g client.c ../msgpipe.c -o client

server: server.c ../msgpipe.c
	gcc -Wall -g server.c ../msgpipe.c -o server