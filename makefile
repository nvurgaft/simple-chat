#makefile for chat server/client applications

all:
	gcc -o client client.c -Wall
	gcc -o server server.c -lpthread -Wall
