#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#include "helpers.h"

#ifndef TFTP_PG_CLIENT_H
#define TFTP_PG_CLIENT_H

#define BUFFER_SIZE	1024
#define PORT 69
#define MAXHOST 512
#define TAM_BUFFER 10
#define MAX_RETRIES 10

class client {
public:
    client();
    void init(int argc, char **argv);
    int sock_client;
    struct sockaddr_in my_addr;	/* for local socket address */
    struct sockaddr_in serv_addr;	/* for server socket address */
    struct in_addr req_addr;
    socklen_t	addrlen, n_retry;
    struct addrinfo hints, *res;

    void send_file(int socket,char* filename,char* mode,struct sockaddr_in clientaddr_in);

    void receiving_file(int socket, char* filename,char * mode,struct sockaddr_in clientaddr_in);

};


#endif //TFTP_PG_CLIENT_H
