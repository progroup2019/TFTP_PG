



#ifndef TFTP_PG_SERVER_H
#define TFTP_PG_SERVER_H
#define  MAX_CLIENTS_CONNECTION 128
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

class server {

public:
    server();

    void init();

    void get_command(char *buffer);

    void server_received_file(char *buffer);

    void server_send_file(char *buffer);

private:

    /*
     * Declared variables
     */
    int socket_server, conexion_cliente, puerto, request;
    socklen_t longc;
    struct sockaddr_in servidor, cliente;
    char buffer[BUFFER_SIZE];
    Header *header;
    fd_set read_mask;

};


#endif //TFTP_PG_SERVER_H