+----------------------------------------------------------------------------------+
| Koby Vurgaft	id: 303829063	login: kobyvo	email: slipperygate@gmail.com      |
+----------------------------------------------------------------------------------+

                          Project 1: Client-Server chat app
                          ---------------------------------
  

Submitted Files
---------------

client.c - the client side for the chat application
server.c - the server side for the chat application
chat.h - header file for both client and server
README - This file, description for this project
MAKEFILE - compile configuration for this project


Brief Details
-------------

A 2-part implementation for a communication application, this project is devided into two(2) main
parts, the client (refered to as the client side) and the server (refered as server side).
the client (alone or with more other identicle clients) attempts to establish a connectiong with the
server and begins communicating with other clients connected to the server side.


Remarks
------------

The client program can exit if a user types 'exit'
The server program can also exit if a connected client user types 'kill server' (with a whitespace) 


TODO
------------

some of the things that should be implemented in the future

1. implement threading for multiple client support
2. save all clients in a linked list and add log options and properties