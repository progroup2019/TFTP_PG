#include <cstdio>
#include <stdlib.h>
#include <fstream>
#include "server.h"
#include "helpers.h"
server::server() {
    
}

void server::init() {

    memset ((char *)&server_addr, 0, sizeof(struct sockaddr_in));
    memset ((char *)&client_addr, 0, sizeof(struct sockaddr_in));

    socket_server = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_server == -1){
        perror("Unable to assign socket UDP \n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if(bind(socket_server, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("Unable to bind address server\n");
        close(socket_server);
        return;
    }


    FD_ZERO(&read_mask);
    FD_SET(socket_server, &read_mask);

    int type_option = -1;
    char* filename;
    while (1){
        if (FD_ISSET(socket_server, &read_mask)) {
            request = recvfrom(socket_server, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&client_addr, &addrlen);

            if (request == -1){
                perror("Error receiving\n");
                return;
                exit(1);
            }

            buffer[request] = '\0';

            char hostname[MAX_CLIENTS_CONNECTION];

            addrlen = sizeof(struct sockaddr_in);


            if(getnameinfo((struct sockaddr *)&client_addr, sizeof(client_addr), hostname, MAX_CLIENTS_CONNECTION, NULL, 0, 0)){
                if (inet_ntop(AF_INET, &(client_addr.sin_addr), hostname, MAX_CLIENTS_CONNECTION) == NULL)
                    perror(" inet_ntop \n");
            }


            type_option = helpers::get_packet_type((BYTE *) buffer);

            //Verify that filename command is correct
            if(type_option == 1){
                filename = helpers::get_filename((BYTE *) buffer);
                server_received_file(filename);
                printf("%s\n", filename);
            }else{
                perror("Error in command to get filename.");
            }

        }
        break;
    }
}


void server::server_received_file(char* filename){
}


