#include <cstdio>
#include <stdlib.h>
#include <fstream>
#include "server.h"
#include "helpers.h"
server::server() {
    memset ((char *)&servidor, 0, sizeof(struct sockaddr_in));
    memset ((char *)&cliente, 0, sizeof(struct sockaddr_in));
    longc = sizeof(struct sockaddr_in);

    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(PORT);
    socket_server = socket(AF_INET, SOCK_DGRAM, 0);

}

void server::init() {
    if(bind(socket_server, (struct sockaddr *)&servidor, sizeof(struct sockaddr_in)) < 0)
    {
        printf("Unable to bind address server\n");
        close(socket_server);
        return;
    }


    FD_ZERO(&read_mask);
    FD_SET(socket_server, &read_mask);

    int type_option = -1;
    char* filename;
    while (1){
        if (FD_ISSET(socket_server, &read_mask)) {
            request = recvfrom(socket_server, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&cliente, &longc);

            if (request == -1){
                printf("Error receiving\n");
                return;
                exit(1);
            }

            buffer[request] = '\0';

            char hostname[MAX_CLIENTS_CONNECTION];

            longc = sizeof(struct sockaddr_in);

            if(getnameinfo((struct sockaddr *)&cliente,sizeof(cliente),hostname,MAX_CLIENTS_CONNECTION,NULL,0,0)){
                if (inet_ntop(AF_INET, &(cliente.sin_addr), hostname, MAX_CLIENTS_CONNECTION) == NULL)
                    perror(" inet_ntop \n");
            }

            type_option = helpers::get_packet_type(reinterpret_cast<BYTE *>(buffer));

            if(type_option == 2){
                filename = helpers::get_filename(reinterpret_cast<BYTE *>(buffer));
                server_received_file(filename);
            }

        }
        break;
    }
}


void server::server_received_file(char* filename){
}


