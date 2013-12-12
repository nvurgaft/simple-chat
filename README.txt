+---------------------------------------------------------------------------------+
| written bynvurgaft                                                              |
+---------------------------------------------------------------------------------+

                    Multithreaded client-server chat program FINAL
                    ----------------------------------------------

Files
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

An attempt to write a multithreaded (POSIX), multi user, CLI chat server and client application under linux with C
1. this version is still uncomplete!!!
2. multiple users can connect but they only can discuss with the server, future code will resolve this so they could
converse with eachother
3. conversation is still uncomplete and blocking


How to use
----------

You should first start the server side application, do this by typing "./server" followed by the port 
number the server should work on, ie, "./server 5000".
Then start a client by typing "./client" followed by the host name andthe port number of the target server, 
ie "./client hostname 5000" (the only hostname currently supported is plain 'hostname')

Compiling
---------

You can use the makefile attached to compile a copy of both client and server executabes

Remarks
------------

1. Currently the conversation loops are unbreakable and exit is ony possible with ctrl+C
2. The program won't handle the event of an invalid port number.
