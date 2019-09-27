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
            } else if (type_option == 1){
                filename = helpers::get_filename(reinterpret_cast<BYTE *>(buffer));
                server_send_file(filename);
            }
        }
        break;
    }
}


void server::server_received_file(char* filename){
}


void server::server_send_file(char *filename) {
    int packet_number = 0, end = 0;
    int byte_received, data_to_send = PACKET_SIZE;
    char * data_file;
    char * last_data_file;
    char *buffer;

    char command[4];

    socklen_t addrlen;
    int size_file, type_option;
    int num_packets;
    int mod_packet;

    FILE *file;

    addrlen = sizeof(struct sockaddr_in);

    file = fopen(filename,"rb");

    if (file == NULL){
        printf("File %s no found\n",filename);
        helpers::ACK_ERROR(socket_server,cliente,1, "File no Found");
        return;
    }


    fseek(file, 0L, SEEK_END);
    size_file=ftell(file);
    num_packets=size_file/PACKET_SIZE;
    mod_packet=size_file%PACKET_SIZE;

    rewind(file);
    data_file = static_cast<char *>(calloc(PACKET_SIZE, sizeof(char)));

    if(mod_packet!=0)
        last_data_file = static_cast<char *>(calloc(mod_packet, sizeof(char)));
    else
        last_data_file = static_cast<char *>(calloc(1, sizeof(char)));

    if(data_file==NULL || last_data_file==NULL){
        printf("Error e lectura de archivo\n");
        helpers::ACK_ERROR(socket_server,cliente,0, "Error reading file");
        fclose (file);
        return;
    }

    while (end != 2){
        packet_number++;
        if(packet_number<=num_packets){
            fread(data_file, PACKET_SIZE,1,file);
            buffer = reinterpret_cast<char *>(helpers::prepare_data_to_send(packet_number,
                                                                            reinterpret_cast<BYTE *>(data_file)));
        }else{
            end=1;
            if (mod_packet){
                fread(last_data_file, mod_packet,1,file);
                data_to_send = mod_packet;
            } else{
                last_data_file[0]=0;
                data_to_send = 1;
            }
            buffer = reinterpret_cast<char *>(helpers::prepare_data_to_send(packet_number,
                                                                            reinterpret_cast<BYTE *>(last_data_file)));
        }
        printf("Enviando %d bytes de bloque %d / %d \n",data_to_send,packet_number,num_packets);
        sendto (socket_server, buffer, 4+data_to_send,0, (struct sockaddr *)&cliente, addrlen);

        byte_received = recvfrom (socket_server, command, 4,0,(struct sockaddr *)&cliente, &addrlen);

        printf("recibiendo %d bytes - opcion %d\n",byte_received,helpers::get_packet_type(reinterpret_cast<BYTE *>(command)));

        if(byte_received == -1){
            printf("Error receiving data\n");
            helpers::ACK_ERROR(socket_server,cliente,0, "Error receiving request");
            fclose (file);
            free(data_file);
            free(last_data_file);
            return;
        }

        type_option = helpers::get_packet_type(reinterpret_cast<BYTE *>(command));

        if(type_option == 5){
            fclose(file);
            perror(helpers::get_data(reinterpret_cast<BYTE *>(command), byte_received));
            return;
        }

        if (type_option != 4){
            printf("Packet type invalid");
            helpers::ACK_ERROR(socket_server,cliente,4, "Packet type invalid");
            fclose(file);
            return;
        }

        if (helpers::get_packet_number(reinterpret_cast<unsigned char *>(command)) != packet_number){
            printf("Bloque incorrecton");
            helpers::ACK_ERROR(socket_server,cliente,4, "Block invalid");
            fclose(file);
            free(data_file);
            free(last_data_file);
            return;
        }

        if (end == 1) end =2;
    }

}
