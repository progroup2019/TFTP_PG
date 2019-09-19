#include <cstdio>
#include <fstream>
#include "server.h"
server::server() {
    header = new Header("6969",6969,550,512,150,99,3,5);
    puerto = 6969;
    conexion_servidor = socket(AF_INET, SOCK_STREAM, 0); //creamos el socket
    bzero((char *)&servidor, sizeof(servidor)); //llenamos la estructura de 0's
    servidor.sin_family = AF_INET; //asignamos a la estructura
    servidor.sin_port = htons(puerto);
    servidor.sin_addr.s_addr = INADDR_ANY; //esta macro especifica nuestra dirección

}

void server::init() {
    if(bind(conexion_servidor, (struct sockaddr *)&servidor, sizeof(servidor)) < 0)
    {
        printf("Error al asociar el puerto a la conexion\n");
        close(conexion_servidor);
        return;
    }
    listen(conexion_servidor, 3); //Estamos a la escucha
    printf("A la escucha en el puerto %d\n", ntohs(servidor.sin_port));
    while (1){
        longc = sizeof(cliente); //Asignamos el tamaño de la estructura a esta variable
        conexion_cliente = accept(conexion_servidor, (struct sockaddr *)&cliente, &longc); //Esperamos una conexion
        if(conexion_cliente<0)
        {
            printf("Error al aceptar trafico\n");
            close(conexion_servidor);
            return;
        }
        printf("Conectando con %s:%d\n", inet_ntoa(cliente.sin_addr),htons(cliente.sin_port));
        if(recv(conexion_cliente, buffer, 100, 0) < 0)
        { //Comenzamos a recibir datos del cliente
            //Si recv() recibe 0 el cliente ha cerrado la conexion. Si es menor que 0 ha habido algún error.
            printf("Error al recibir los datos\n");
            close(conexion_cliente);
        }
        else
        {
            get_command(buffer);
            bzero((char *)&buffer, sizeof(buffer));
        }
    }

    close(conexion_servidor);
}


void server::get_command(char * buffer) {
    int numbytes;

    if(buffer[0] == '0' && buffer[1] == '1'){
        /**
         * RRQ
         */

    }else if (buffer[0] == '0' && buffer[1] == '2'){
        /**
         * WRQ
         */
         /**
          * prepare ACK
          */
        char *message = header->make_ack("00");
        if((numbytes = send(conexion_cliente,message,strlen(message),0)) == -1){
            perror("SERVER ACK: sendto");
            //exit(1);
        }

        /**
         * get filename
        */
        char filename[header->get_max_filename_len()];
        strcpy(filename, buffer+2);

        /**
         * checking if exists file
        */
        if(access(filename, F_OK) != -1){ //SENDING ERROR PACKET - DUPLICATE FILE
            fprintf(stderr,"SERVER: file %s already exists, sending error packet\n", filename);
            char *e_msg = header->make_err("06", "ERROR_FILE_ALREADY_EXISTS");
            send(conexion_cliente,e_msg,strlen(e_msg),0);
            exit(1);
        }

        FILE *fp = fopen(filename, "wt");
        if(fp == NULL || access(filename, W_OK) == -1){ //SENDING ERROR PACKET - ACCESS DENIED
            fprintf(stderr,"SERVER: file %s access denied, sending error packet\n", filename);
            char *e_msg = header->make_err("05", "ERROR_ACCESS_DENIED");
            send(conexion_cliente,e_msg,strlen(e_msg),0);
            exit(1);
        }

        int written_file;
        char buffer_file[4+header->get_max_read_len()];
        fclose(fp);
        while(1){
            if (recv(conexion_cliente, buffer_file, strlen(buffer_file), 0) < 0){
                close(conexion_cliente);
                fclose(fp);
                break;
            } else{
                if (buffer_file[0] == '0' && buffer_file[1] == '3')
                {
                    std::ofstream outfile (filename,std::ofstream::out);
                    outfile.write(buffer_file+4,strlen(buffer_file));
                    outfile.close();
                    char *message = header->make_ack("00");
                    if((numbytes = send(conexion_cliente,message,strlen(message),0)) == -1){
                        perror("SERVER ACK: sendto");
                        close(conexion_cliente);
                        fclose(fp);
                        break;
                    }
                }else{
                    char *e_msg = header->make_err("04", "ERROR_ACCESS_DENIED");
                    send(conexion_cliente,e_msg,strlen(e_msg),0);
                    close(conexion_cliente);
                    fclose(fp);
                    break;
                }
                if (strlen(buffer_file) < header->get_max_read_len()+4)
                {
                    fclose(fp);
                    break;
                }

            }
        }

    } else{
        /**
         * wrong command
         */
    }
}
