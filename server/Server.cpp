#include "Server.h"

Server::Server(){
    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(5000);
    if((serverSocket=socket(AF_INET,SOCK_STREAM,0))==-1)
        printf("Error creating socket.\n");
    if(bind(serverSocket,(sockaddr*)&servaddr,sizeof(servaddr))==-1)
        printf("There was an error binding.\n");
    else{
        printf("%s: Server is created on socket %d.\n",getTime(),serverSocket);
        log = fopen("log.txt","a+");
        fprintf(log,"%s: Server is created on socket %d.\n",getTime(),serverSocket);
        fclose(log);
        log=0;
        fcntl(serverSocket,F_SETFL,O_NONBLOCK);
        isActive=true;
    }
    if(listen(serverSocket,10)==-1)
        printf("There is an error with listening on socket.\n");
    agents=0;
}

int Server::getConnection(){
    sockaddr temp;
    socklen_t num = sizeof(temp);
    int newSock = accept(serverSocket,&temp,&num);
    if(newSock!=-1){
        char buf[20];
        memset(buf,0,sizeof(buf));
        int total = recv(newSock,buf,sizeof(buf),0);
        //printf("received %d\r\n",strlen(buf));
        char buf1[INET_ADDRSTRLEN];
        getIp(&temp,buf1);
        log = fopen("log.txt","a+");
        printf("%s: Received a \"%s\" action from %s\n",getTime(),buf,buf1);
        fprintf(log,"%s: Received a \"%s\" action from %s\n",getTime(),buf,buf1);
        fclose(log);
        log=0;

        // Handle events based on strings received.
        // A safe way to exit server is to provide it with "#quit"
        if(!strcmp("#QUIT",buf))
            quit(&temp,newSock);
        else if(!strcmp(buf,"#JOIN"))
            join(&temp,newSock);
        else if(!strcmp(buf,"#LIST"))
            list(&temp,newSock);
        else if(!strcmp(buf,"#LEAVE"))
            leave(&temp,newSock);
        else if(!strcmp(buf,"#LOG"))
            logFunction(&temp,newSock);
    }
    if(newSock!=-1) close(newSock);
    return newSock;
}

Server::~Server(){
    close(serverSocket);
    if(agents) delete [] agents;
    if(log){ fclose(log); log=0;}
}

bool Server::active(){
    return isActive;
}

void Server::resize(){
    if(agents==0){
        agents = new activeAgents;
        memset(agents->ip,0,sizeof(agents->ip));
        numAgentsActive++;
    }
    else{
        activeAgents *p;
        p = new activeAgents[numAgentsActive+1];
        for(int i = 0; i < numAgentsActive; i++)
            p[i] = agents[i];
        delete [] agents;
        agents = p;
        numAgentsActive++;
        memset(agents[numAgentsActive-1].ip,0,sizeof(agents[numAgentsActive-1].ip));
    }
}

int Server::search(char *str){
    int index = -1;
    if(agents)
        for(int i = 0; i<numAgentsActive; i++){
            if(!strcmp(agents[i].ip,str))
                index = i;
        }
    return index;
}

void *Server::getIp(sockaddr *s, char *rs){
    memset(rs,0,INET_ADDRSTRLEN);
    //printf("Size of %d\n",INET_ADDRSTRLEN);
    sockaddr_in *ipv4 = (sockaddr_in*)s;
    inet_ntop(AF_INET,&(ipv4->sin_addr),rs,INET_ADDRSTRLEN);
}

void Server::join(sockaddr* s,int sock){
    char tempStr[20],tempStr1[20];
    memset(tempStr,0,sizeof(tempStr));
    getIp(s,tempStr1);
    strcpy(tempStr,tempStr1);
    int find = search(tempStr);
    log = fopen("log.txt","a+");
    if(find==-1){
        resize();
        strcpy(agents[numAgentsActive-1].ip,tempStr);
        agents[numAgentsActive-1].startTime = clock();
        fprintf(log,"%s: Responded to agent %s with \"$OK\"\n",getTime(),tempStr);
        printf("%s: Responded to agent %s with \"$OK\"\n",getTime(),tempStr);
        char str[] = "$OK";
        write(sock,str,strlen(str));
    }
    else{
        fprintf(log,"%s: No response is supplied to agent %s (agent already active)\n",getTime(),tempStr);
        printf("%s: No response is supplied to agent %s (agent already active)\n",getTime(),tempStr);
    }
    fclose(log);
    log=0;
}

void Server::list(sockaddr *info, int sock){
    log = fopen("log.txt","a+");
    char ipStr[INET_ADDRSTRLEN];
    memset(ipStr,0,sizeof(ipStr));
    sockaddr_in *temp1 = (sockaddr_in*)info;
    inet_ntop(AF_INET,&(temp1->sin_addr),ipStr,INET_ADDRSTRLEN);
    if(!agents){
        fprintf(log,"%s: No response is supplied to agent %s (no list to send)\n",getTime(),ipStr);
        printf("%s: No response is supplied to agent %s (no list to send)\n",getTime(),ipStr);
        return;
    }
    if(search(ipStr)!=-1){
        for(int i = 0; i < numAgentsActive; i++){
            char str[200];
            memset(str,0,sizeof(str));
            clock_t timeActive = (clock()-agents[i].startTime);
            int secs = (timeActive/CLOCKS_PER_SEC);
            sprintf(str,"\nIp address: %s\nTime active (secs): %d\n\n",agents[i].ip,secs);
            write(sock,str,strlen(str));
        }
        fprintf(log,"%s: Responded to agent %s with list of active agents.\n",getTime(),ipStr);
        printf("%s: Responded to agent %s with list of active agents.\n",getTime(),ipStr);
    }
    else{
        fprintf(log,"%s: No response is supplied to agent %s (agent not active)\n",getTime(),ipStr);
        printf("%s: No response is supplied to agent %s (agent not active)\n",getTime(),ipStr);
    }
    fclose(log);
    log=0;
}

void Server::leave(sockaddr* s, int sock){
    if(!agents){
        char str[] = "No computers active.";
        printf("%s\n",str);
        write(sock,str,strlen(str));
        return;
    }
    char ipStr[INET_ADDRSTRLEN];
    memset(ipStr,0,sizeof(ipStr));
    sockaddr_in *temp1 = (sockaddr_in*)s;
    inet_ntop(AF_INET,&(temp1->sin_addr),ipStr,INET_ADDRSTRLEN);
    int find = search(ipStr);
    if(find==-1){
        log = fopen("log.txt","a+");
        fprintf(log,"%s: No response is supplied to agent %s (agent not active)\n",getTime(),ipStr);
        printf("%s: No response is supplied to agent %s (agent not active)\n",getTime(),ipStr);
        fclose(log);
        log=0;
        return;
    }
    else{
        if(numAgentsActive-1 == 0){
            delete [] agents;
            log = fopen("log.txt","a+");
            fprintf(log,"%s: Responded to agent %s with \"$OK\"\n",getTime(),ipStr);
            fclose(log);
            log=0;
            printf("%s: Responded to agent %s with \"$OK\"\n",getTime(),ipStr);
            char str[] = "$OK";
            write(sock,str,strlen(str));
            agents=0;
            return;
        }
        activeAgents *p = new activeAgents[numAgentsActive-1];
        for(int i = 0; i<find; i++)
            p[i] = agents[i];
        for(int i = find; i < numAgentsActive-1; i++)
            p[i] = agents[i+1];
        delete [] agents;
        agents = p;
        numAgentsActive--;
        log = fopen("log.txt","a+");
        fprintf(log,"%s: Responded to agent %s with \"$OK\"\n",getTime(),ipStr);
        printf("%s: Responded to agent %s with \"$OK\"\n",getTime(),ipStr);
        fclose(log);
        log=0;
        char str[] = "$OK";
        write(sock,str,strlen(str));
    }
    if(log){ fclose(log);log=0;}
    log=0;
}

const char *Server::getTime(){
    time_t nowTime;
    struct tm * info;
    time(&nowTime);
    info = localtime(&nowTime);
    return asctime(info);
}

void Server::quit(sockaddr* s, int sock){
    char buf2[INET_ADDRSTRLEN];
    getIp(s,buf2);
    log = fopen("log.txt","a+");
    if(agents==0){
        printf("%s: Responded to agent %s with \"$OK\"",getTime(),buf2);
        fprintf(log,"%s: Responded to agent %s with \"$OK\"",getTime(),buf2);
        char str[] = "$OK";
        write(sock,str,strlen(str));
        isActive=false;
        return;
    }
    if(search(buf2)!=-1){
        printf("%s: Responded to agent %s with \"$OK\"",getTime(),buf2);
        fprintf(log,"%s: Responded to agent %s with \"$OK\"",getTime(),buf2);
        char str[] = "$OK";
        write(sock,str,strlen(str));
        isActive=false;
    }
    else{
        fprintf(log,"%s: No response is supplied to agent %s (agent not active)\n",getTime(),buf2);
        printf("%s: No response is supplied to agent %s (agent not active)\n",getTime(),buf2);
    }
    fclose(log);
    log=0;
}

void Server::logFunction(sockaddr* s, int sock){
    log = fopen("log.txt","a+");
    char ipStr[INET_ADDRSTRLEN];
    getIp(s,ipStr);
    if(!agents){
        fprintf(log,"%s: No response is supplied to agent %s (agent not active)\n",getTime(),ipStr);
        printf("%s: No response is supplied to agent %s (agent not active)\n",getTime(),ipStr);
        return;
    }
    int find = search(ipStr);
    if(find==-1){
        fprintf(log,"%s: No response is supplied to agent %s (agent not active)\n",getTime(),ipStr);
        printf("%s: No response is supplied to agent %s (agent not active)\n",getTime(),ipStr);
        return;
    }
    fprintf(log,"%s: Responded to agent %s with \"log.txt\"\n",getTime(),ipStr);
    printf("%s: Responded to agent %s with \"log.txt\"\n",getTime(),ipStr);
    fclose(log);
    log=0;
    log = fopen("log.txt","r");
    char sendArray[100];
    bool stillSending = true;
    while(stillSending){
        memset(sendArray,0,100);
        fgets(sendArray,100,log);
        if(!strcmp(sendArray,"")){
            stillSending=false;
        }
        else{
            write(sock,sendArray,strlen(sendArray));
        }
    }
    fclose(log);
    log=0;
}
