# Client-and-Server-Application-Sample
University project to learn the basics of socket API.
# Notes
## Introduction
The supplied program was tested on Linux and Windows running Cygwin. The client was written in C, and the server was written in C++ (To encapsulate some complexity). The makefile should suffice to compile program. Run "make" from main directory. The executables will be generated in client and server subdirectories as appropriate. The command for the server is just "./server" . This is assuming "server" is the name of the executable. If the makefile does not work, please see its actual contents to identify the commands that I used to compile. The command for the client is "./agent <ip> <port> <server_command>" . I found that the server_command argument is better interpreted by the program if surrounded by quotes. For example, "#JOIN" (in this case, do not ignore the quotes).
The existing log.txt file is a snapshot of program results as run on Saturday December 1, 2018. If server program is run, the application is designed such that new log information is appended. Therefore, for unique information for subsequent runs, the log file will have to be deleted manually. By default, the server runs on port 5000.
## Structures
The program is somewhat modularized. A single class holding the server information is provided with a single public function that does all of the work. Within this class, a structure was designed to hold IP of a client, as well as its start time for use in certain calculations. There are many header files included in the Server.h file. Linux should have them all.
## Valid Commands
These are the valid commands that the server can accept:
* "#JOIN": Adds a client's IP to the server list
* "#LEAVE": Removes the client from the list maintained on server (if client exists)
* "#LOG": Sends the information stored in log.txt to the client (client has to be in the list maintained by the server)
* "#LIST": Sends a list of active agents to the client with ip address and time in seconds of client's existence on the server (client has to be in the list maintained by the server)
* "#QUIT": Issues a quit request for the server to shut down properly as opposed to being interupted ((client has to be in the list maintained by the server)
