

#ifndef TFTP_PG_SERVER_H
#define TFTP_PG_SERVER_H
#define MAX_CLIENTS_CONNECTION 128
#define BUFFER_SIZE	1024
#define PORT 6969

#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE	1024
#define PORT 6969

class server {

public:
    int socket_server, request;
    struct sockaddr_in server_addr, client_addr;
    fd_set read_mask;
    char buffer[BUFFER_SIZE];
    socklen_t addrlen;

    server();

    void init();

    void get_command(char *buffer);

    void server_received_file(char *buffer);

private:

    /*
     * Declared variables
     */
    int conexion_cliente, puerto;
    
    Header *header;
    

};


#endif //TFTP_PG_SERVER_H
