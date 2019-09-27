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
                printf("Iniciando petición de escritura del archivo: %s\n", filename);
                server_received_file(filename);           
            }else{
                perror("Ilegal operation.");
            }

        }
        break;
    }
}


void server::server_received_file(char* filename){
    int packedtNumber = 0;
    int request;

    char *packet;
    char filePiece[PACKET_SIZE + 4];
    
    FILE *file;

    char fileRoute[25] = "server_files/";
    strcat(fileRoute, filename);

    file = fopen(fileRoute, "rb");
    if(file != NULL){
        fclose(file);
        perror("Error: File already exists");
        return;
    }

    file = fopen(fileRoute, "wb");
    packet = (char *) helpers::make_ACK(0) ;

	for(int i=0; i<4; i++)
		printf(" %d ", packet[i]);
	printf("\n");
    printf("socket; %d \n", socket_server);
    int x =sendto(socket_server, packet , 4, 0, (struct sockaddr *)&client_addr, addrlen);
    printf("x = %d\n", x);
    if( x == -1)
    {
        perror("ACK ERROR");
    }
    fclose(file);


}


