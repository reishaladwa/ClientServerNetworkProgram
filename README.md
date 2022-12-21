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
start the server by typing the following at a linux prompt:

``` ./server PFE.csv MRNA.csv 30000```

The last argument is the port number that the server will listen to.

## Starting the Client
If we assume that the name of the client’s compiled executable is “client” then you would start
the client by typing the following at a linux prompt:

```./client localhost 30000```

The first argument is the domain name of the machine that server is running on.

## User Interface

An example of a user interaction with the client is shown below.

```PricesOnDate 2020-05-08```

PFE: 202.90 | MRNA: 38.58



```MaxPossible profit PFE 2019-09-11 2019-10-15```

14.41



```MaxPossible loss MRNA 2020-04-16 2020-08-23```

6.37
