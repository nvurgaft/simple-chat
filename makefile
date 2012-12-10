#makefile

all:
	gcc -o client client.c
	gcc -o server server.c -lpthread