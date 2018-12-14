all:
	g++ server/Server.cpp server/main.cpp -o server/server
	gcc -w client/agent.c -o client/agent
