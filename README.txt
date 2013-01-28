+----------------------------------------------------------------------------------+
| Koby Vurgaft	id: 303829063	login: kobyvo	email: slipperygate@gmail.com      |
+----------------------------------------------------------------------------------+

                    Project 3: Multithreaded client-server chat program FINAL
                    ---------------------------------------------------------
  

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


How to use
----------

You should first start the server side application, do this by typing "./server" followed by the port 
number the server should work on, ie, "./server 5000".
Then start a client by typing "./client" followed by the host name andthe port number of the target server, 
ie "./client hostname 5000".


Remarks
------------

1. The client program will exit if a user types 'exit', it's corresponding server thread would end too.
2. The program won't handle the event of an invalid port number.