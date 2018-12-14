#ifndef _SERVER_H
#define _SERVER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

struct activeAgents{
    char ip[20];
    clock_t startTime;
};

class Server{
private:
    int serverSocket;
    bool isActive;
    activeAgents *agents;
    int numAgentsActive=0;
    FILE *log;
    void resize();
    int search(char *);
    void join(sockaddr*,int);
    void *getIp(sockaddr*,char*);
    void list(sockaddr*,int);
    void leave(sockaddr*,int);
    const char *getTime();
    void quit(sockaddr*,int);
    void logFunction(sockaddr*,int);
public:
    Server();
    ~Server();
    int getConnection();
    bool active();
};
#endif
