# Client/Server Network Program

## Description

This is a networked client/server application which allows a client program to query a
server program to fetch stock market information. The program allows a user to examine
stock prices for two major stocks namely Pfizer(PFE) and Moderna(MRNA). The user will enter
commands into the client program which runs on one machine. For each command entered by
the user, the client program sends a message containing the query command. The server
program receives the query and responds with a message containing the requested
information. 

## Starting the Server
If we assume that the name of the server’s compiled executable is “server” then you would
start the server by typing the following at a linux prompt,
“./server PFE.csv MRNA.csv 30000”
