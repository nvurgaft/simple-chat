+----------------------------------------------------------------------------------+
| Koby Vurgaft	id: 303829063	login: kobyvo	email: slipperygate@gmail.com      |
+----------------------------------------------------------------------------------+

                    Project 2: Multithreaded client-server chat program
                    ---------------------------------------------------
  

Submitted Files
---------------

client.c - the client side for the chat application
server.c - the server side for the chat application
chat.h - header file for both client and server
dbllist.c - a doubly linked list used to store client names in the server
dbllist.h - header file for the linked list 
README - This file, description for this project
MAKEFILE - compile file for this project (in terminal run 'make' to compile)


Brief Details
-------------

A 2-part implementation for a multithreaded chat application, this project is devided into two(2) 
parts, the client side and the server side.
the client (alone or with more other identicle clients) attempts to establish a connectiong with the
server, when accepted it connects to other clients connected to the server and startes communication
with it.


Remarks
------------

The client program will exit if a user types 'exit', it's corresponding server thread would end too.