#makefile for chat server/client applications

all:
	gcc -o client client.c -lpthread
	gcc -o server server.c -lpthread -Wall
