#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include "libs/client.h"
int main(int argc, char **argv){
    client *c = new client();
    c->init(argc,argv);

}

