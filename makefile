#makefile for chat server/client applications

all:
	gcc -o client client.c
	gcc -o server server.c -lpthread