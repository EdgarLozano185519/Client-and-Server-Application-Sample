#include "Server.h"

int main(int argc, char **argv){
    Server server;
    while(server.active()){
        int a = server.getConnection();
    }
}
