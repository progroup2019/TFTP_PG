#include "client.h"
client::client() {


}

void client::init(int argc, char **argv) {
    servidor = gethostbyname(argv[1]); //Asignacion
    if(servidor == NULL)
    { //Comprobación
        printf("Host erróneo\n");
        return;
    }
    conexion = socket(AF_INET, SOCK_STREAM, 0); //Asignación del socket
    puerto=(atoi(argv[2])); //conversion del argumento
    bzero((char *)&cliente, sizeof((char *)&cliente)); //Rellena toda la estructura de 0's
    //La función bzero() es como memset() pero inicializando a 0 todas la variables
    cliente.sin_family = AF_INET; //asignacion del protocolo
    cliente.sin_port = htons(puerto); //asignacion del puerto
    bcopy((char *)servidor->h_addr, (char *)&cliente.sin_addr.s_addr, sizeof(servidor->h_length));
    //bcopy(); copia los datos del primer elemendo en el segundo con el tamaño máximo
    //del tercer argumento.


    //cliente.sin_addr = *((struct in_addr *)servidor->h_addr); //<--para empezar prefiero que se usen
    //inet_aton(argv[1],&cliente.sin_addr); //<--alguna de estas dos funciones
    if(connect(conexion,(struct sockaddr *)&cliente, sizeof(cliente)) < 0)
    { //conectando con el host
        printf("Error conectando con el host\n");
        close(conexion);
        return;
    }
    printf("Conectado con %s:%d\n",inet_ntoa(cliente.sin_addr),htons(cliente.sin_port));
    //inet_ntoa(); está definida en <arpa/inet.h>
    printf("Escribe un mensaje: ");
    fgets(buffer, 100, stdin);
    send(conexion, buffer, 100, 0); //envio
    bzero(buffer, 100);
    recv(conexion, buffer, 100, 0); //recepción
    printf("%s", buffer);
    return;
}
