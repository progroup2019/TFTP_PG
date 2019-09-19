#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#ifndef TFTP_PG_CLIENT_H
#define TFTP_PG_CLIENT_H


class client {
public:
    client();
    void init(int argc, char **argv);
    struct sockaddr_in cliente; //Declaración de la estructura con información para la conexión
    struct hostent *servidor; //Declaración de la estructura con información del host
    int puerto, conexion;
    char buffer[100];

};


#endif //TFTP_PG_CLIENT_H
