#include <cstdio>
#include <stdlib.h>
#include <fstream>
#include "server.h"
#include "helpers.h"
#include <iostream>
server::server() {
    memset ((char *)&servidor, 0, sizeof(struct sockaddr_in));
    memset ((char *)&cliente, 0, sizeof(struct sockaddr_in));
    longc = sizeof(struct sockaddr_in);


    printf("Seleccione una IP para selecionar servidor\n");
    helpers *h = new helpers();
    h->get_my_ipv4();
    int optip;
    std::cin>>optip;
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = h->ipsI[optip];
    servidor.sin_port = htons(PORT);
    socket_server = socket(AF_INET, SOCK_DGRAM, 0);

}

void server::init() {
    if(bind(socket_server, (struct sockaddr *)&servidor, sizeof(struct sockaddr_in)) < 0)
    {
        printf("Unable to bind address server, run serve in mode super admin \n");
        close(socket_server);
        return;
    }

    struct sockaddr_in my_addr;
    char myIP[16];
    unsigned int myPort;
    bzero(&my_addr, sizeof(my_addr));
    socklen_t len = sizeof(my_addr);

    getsockname(socket_server, (struct sockaddr *) &my_addr, &len);
    inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
    myPort = ntohs(my_addr.sin_port);

    printf("Local ip address: %s\n", myIP);
    printf("Local port : %u\n", myPort);


    FD_ZERO(&read_mask);
    FD_SET(socket_server, &read_mask);

    int type_option = -1;
    char* filename;
    while (1){
        printf("Esperando cliente\n");
        if(select(socket_server+1, &read_mask, (fd_set *)0, (fd_set *)0, NULL))
        {
            if (FD_ISSET(socket_server, &read_mask)) {
                printf("Cliente conectado\n");
                request = recvfrom(socket_server, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&cliente, &longc);

                if (request == -1){
                    printf("Error receiving\n");
                    return;
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
                } else {
                    helpers::ACK_ERROR(socket_server,cliente,0,"Command invalid");
                }
            }
        }else{
            printf("End server\n");
            close(socket_server);
            exit(1);            
        }
        printf("trasnferencia finalizada\n");
    }

    close(socket_server);
}


void server::server_received_file(char* filename){
    printf("Start receiving file %s\n", filename);
    int packet_number= 0, end= 0;
    int bytes_received,type_option;
    char * buffer;
    char data_file[PACKET_SIZE+4];

    FILE *file;

    socklen_t addrlen;
    addrlen = sizeof(struct sockaddr_in);

    file = fopen(filename,"rb");

    if (file != NULL){
        helpers::ACK_ERROR(socket_server,cliente,1, "File exists");
        fclose(file);
        printf("File %s exists\n",filename);
        return;
    }
    file = fopen(filename,"wb");
    buffer = reinterpret_cast<char *>(helpers::ACK(0));

    printf("Sending ACK 0401 \n");
    sendto(socket_server,buffer,4,0, (struct sockaddr *)&cliente, addrlen);

    int ant = 0;
    while (end!=2){
        packet_number++;
        if (packet_number == (256*256-1))
        {
            ant = packet_number + ant;
            packet_number = 0;
        }

        bytes_received = recvfrom(socket_server,data_file,PACKET_SIZE+4,0,(struct sockaddr *)&cliente, &addrlen);


        printf("%d bytes received  of packet:%d \n",bytes_received,packet_number);

        if(bytes_received == -1){
            printf("Error receiving data\n");
            helpers::ACK_ERROR(socket_server,cliente,0,"Error receiving data");
            fclose(file);
            return;
        }

        type_option = helpers::get_packet_type(reinterpret_cast<BYTE *>(data_file));

        if(type_option == 5){
            fclose(file);
            perror(helpers::get_data(reinterpret_cast<BYTE *>(data_file), bytes_received));
            return;
        }

        if (type_option != 3){
            printf("Packet type invalid");
            helpers::ACK_ERROR(socket_server,cliente,4,"Invalid packet");
            fclose(file);
            return;
        }

        if (helpers::get_packet_number(reinterpret_cast<unsigned char *>(data_file)) != packet_number){
            printf("Bloque incorrecton");
            helpers::ACK_ERROR(socket_server,cliente,4,"Invalid packet");
            fclose(file);
            return;
        }

        fwrite(helpers::get_data(reinterpret_cast<BYTE *>(data_file), bytes_received-4), bytes_received-4, 1, file);

        buffer = reinterpret_cast<char *>(helpers::ACK(packet_number));
        printf("Sending ACK for block number %d\n",helpers::get_packet_number(reinterpret_cast<unsigned char *>(data_file)));
        sendto (socket_server, buffer, 4,0, (struct sockaddr *)&cliente, addrlen);
        if (bytes_received-4 < PACKET_SIZE) end = 2;
    }

    fclose(file);
    printf("File received \n");
    return;
}


void server::server_send_file(char *filename) {
    printf("Init send file %s \n",filename);
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
        helpers::ACK_ERROR(socket_server,cliente,6, "File no Found");
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

    int retries = 0;


    int ant = 0;

    while (end != 2){
        if (retries == 0){
            packet_number++;
            if (packet_number == (256*256-1))
            {
                ant = packet_number + ant;
                packet_number = 0;
            }
        } else {
            if (retries >= MAX_RETRIES){
                printf("Maximo de intentos, trasnferencia cancelada\n");
                return;
            }
                
        }
        if((packet_number+ant)<=num_packets){
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
        printf("Sending block number %d\n",packet_number);
        if ((sendto (socket_server, buffer, 4+data_to_send,0, (struct sockaddr *)&cliente, addrlen)) == -1){
            retries++;
            printf("retrie %d\n",retries);
            continue;
        }

        retries = 0;


        byte_received = recvfrom (socket_server, command, 4,0,(struct sockaddr *)&cliente, &addrlen);


        printf("Receiving ack\n");

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
            printf("Incorrect block %d\n",helpers::get_packet_number(reinterpret_cast<unsigned char *>(command)));
            helpers::ACK_ERROR(socket_server,cliente,4, "Block invalid");
            fclose(file);
            free(data_file);
            free(last_data_file);
            return;
        }

        if (end == 1) end =2;
    }

    printf("File sent success\n");
}
