#include <cstdio>
#include <stdlib.h>
#include <fstream>
#include "server.h"
#include "helpers.h"
server::server() {
    // header = new Header("6969",6969,550,512,150,99,3,5);
    memset ((char *)&servidor, 0, sizeof(struct sockaddr_in));
    memset ((char *)&cliente, 0, sizeof(struct sockaddr_in));
    longc = sizeof(struct sockaddr_in);

    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(6969);

    conexion_servidor = socket(AF_INET, SOCK_DGRAM, 0); //creamos el socket

}

void server::init() {
    if(bind(conexion_servidor, (struct sockaddr *)&servidor, sizeof(struct sockaddr_in)) < 0)
    {
        printf("Error al asociar el puerto a la conexion\n");
        close(conexion_servidor);
        return;
    }


    FD_ZERO(&read_mask);
    FD_SET(conexion_servidor, &read_mask);
    int type_option = -1;
    char* filename;
    while (1){
        if (FD_ISSET(conexion_servidor, &read_mask)) {
            int request = recvfrom(conexion_servidor, buffer, BUFFER_SIZE -1, 0,(struct sockaddr *)&cliente, &longc);

            if (request == -1){
                printf("Error recibiendo\n");
                return;
            }

            buffer[request] = '\0';

            char hostname[MAX_CLIENTS_CONNECTION];

            if(getnameinfo((struct sockaddr *)&cliente,sizeof(cliente),hostname,MAX_CLIENTS_CONNECTION,NULL,0,0)){
                if (inet_ntop(AF_INET, &(cliente.sin_addr), hostname, MAX_CLIENTS_CONNECTION) == NULL)
                    perror(" inet_ntop \n");
            }

            type_option = helpers::get_packet_type(buffer);

            if(type_option == 2){
                filename = helpers::get_filename(buffer);
                server_get_file(filename);
            }

        }
        break;
    }
}


void server::server_get_file(char* filename){
    printf("get_file %s",filename);
    FILE * fichero;
    int addrlen = sizeof(struct sockaddr_in);
    int cc;
    char buffer_file[BUFFER_SIZE+4];
    fichero = fopen(filename,"rb");
    if (fichero !=NULL){
        char *e_msg = Header::make_err("06", "File already exists");
        fclose(fichero);
        int ack = sendto (conexion_servidor, e_msg, BUFFER_SIZE, 0, (struct sockaddr *)&cliente, addrlen);
        if ( ack == -1) {
            perror("Error send ACK");
            return;
        }
    }

    fichero = fopen(filename,"wb");
    char* ack_msg = Header::make_ack("0");
    if(sendto (conexion_servidor, ack_msg, BUFFER_SIZE, 0, (struct sockaddr *)&cliente, addrlen) == -1){
        perror("Error send ACK");
        return;
    }

    bool end = false;
    int packet_number = 0;
    while (1){
        packet_number++;
        cc = recvfrom(conexion_servidor, buffer_file, BUFFER_SIZE+4, 0,(struct sockaddr *)&cliente, &longc);
        if(helpers::get_packet_type(buffer_file)!=3){
            fclose(fichero);
            char *e_msg = Header::make_err("04", "Tipo de paquete no esperado");
            int ack = sendto (conexion_servidor, e_msg, BUFFER_SIZE, 0, (struct sockaddr *)&cliente, addrlen);
            if ( ack == -1) {
                perror("Error send ACK");
                return;
            }
            perror("Error tipo de paquete no esperado");
            return;
        }
        printf("%s",buffer_file);
        if(helpers::get_packet_number(reinterpret_cast<unsigned char *>(buffer_file)) != packet_number){
            fclose(fichero);
            char *e_msg = Header::make_err("04", "Numero de bloque invalido");
            int ack = sendto (conexion_servidor, e_msg, BUFFER_SIZE, 0, (struct sockaddr *)&cliente, addrlen);
            if ( ack == -1) {
                perror("Error send ACK");
                return;
            }
            perror("Numero de bloque invalido");
            return;
        }

        fwrite(helpers::get_data(buffer_file,cc-4),cc-4,1,fichero);

        char* ack_msg = Header::make_ack(reinterpret_cast<char *>(packet_number));
        if(sendto (conexion_servidor, ack_msg, BUFFER_SIZE, 0, (struct sockaddr *)&cliente, addrlen) == -1){
            perror("Error send ACK");
            return;
        }

        fclose(fichero);
        break;
    }
}


