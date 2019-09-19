

#ifndef TFTP_PG_SERVER_H
#define TFTP_PG_SERVER_H
#define  MAX_CLIENTS_CONNECTION 10

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

private:

    /*
     * Declared variables
     */
    int conexion_servidor, conexion_cliente, puerto;
    socklen_t longc;
    struct sockaddr_in servidor, cliente;
    char buffer[100];
    Header *header;

};


#endif //TFTP_PG_SERVER_H
