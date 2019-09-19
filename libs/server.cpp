#include <cstdio>
#include "server.h"
server::server() {
    Header *header = new Header("6969",6969,550,512,150,99,3,5);
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
            close(conexion_servidor);
        }
        else
        {
            printf("%s\n", buffer);
            bzero((char *)&buffer, sizeof(buffer));
            send(conexion_cliente, "Recibido\n", 13, 0);
        }
    }

    close(conexion_servidor);
}


void server::get_command(char * buffer) {

}
