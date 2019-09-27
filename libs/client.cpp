#include "client.h"
#include "helpers.h"
client::client() {


}

void client::init(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage:  %s <nameserver>  <GET/POST> <filename>\n", argv[0]);
        exit(1);
    }

    if(!strcmp(argv[2], "GET") )
        if(!strcmp(argv[2], "POST")){
            fprintf(stderr, "Mode should be <GET> or <POST>\n");
            exit(1);
        }



    sock_client = socket (AF_INET, SOCK_DGRAM, 0);
    if (sock_client == -1) {
        perror(argv[0]);
        fprintf(stderr, "%s: unable to create socket\n", argv[0]);
        exit(1);
    }

    memset ((char *)&my_addr, 0, sizeof(struct sockaddr_in));
    memset ((char *)&serv_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = PORT;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    /*
    if (bind(sock_client, (const struct sockaddr *) &my_addr, sizeof(struct sockaddr_in)) == -1) {
        perror(argv[0]);
        fprintf(stderr, "%s: unable to bind socket\n", argv[0]);
        exit(1);
    }
    */


    addrlen = sizeof(struct sockaddr_in);
    if (getsockname(sock_client, (struct sockaddr *)&my_addr, &addrlen) == -1) {
        perror(argv[0]);
        fprintf(stderr, "%s: unable to read socket address\n", argv[0]);
        exit(1);
    }


    serv_addr.sin_family = AF_INET;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_INET;

    if (getaddrinfo (argv[1], NULL, &hints, &res) != 0){
        fprintf(stderr, "%s: No es posible resolver la IP de %s\n",
                argv[0], argv[1]);
        exit(1);
    }
    else {
        /* Copy address of host */
        serv_addr.sin_addr = ((struct sockaddr_in *) res->ai_addr)->sin_addr;
    }
    freeaddrinfo(res);

    serv_addr.sin_port = htons(PORT);


    BYTE *msg;

    if (strcmp(argv[2], "POST"))
    {
        send_file(sock_client,argv[3],"octec",serv_addr);
    }

}

void client::send_file(int socket, char *filename, char *mode, struct sockaddr_in clientaddr_in) {

    int packet_number= 0, end= 0;

    int bytes_received, type_option;

    char * data_file;
    char * last_data_file;
    char * buffer;

    char command[4];

    int data_to_send = PACKET_SIZE;

    int size_file;
    int num_packets;
    int mod_packets;

    FILE *file;

    file = fopen(filename,"rb");
    if(file==NULL){
        printf("No se ha encontrado el fichero %s\n", filename);
        return;
    }

    buffer = reinterpret_cast<char *>(helpers::WRQ_command(filename, mode));


    sendto(socket,buffer,2+strlen(filename)+1+strlen(mode)+1,0,(struct sockaddr *)&clientaddr_in, addrlen);

    bytes_received = recvfrom(socket,command,4,0,(struct sockaddr *)&clientaddr_in, &addrlen);

    if(bytes_received == -1){
        printf("Error al recibir un mensaje\n");
    }

    type_option = helpers::get_packet_type(reinterpret_cast<BYTE *>(command));

    if (type_option == 5){
        printf("%s\n",command);
        fclose(file);
    }
    if(type_option != 4){
        printf("Se esperaba ack\n");
        fclose(file);
        return;
    }

    if(helpers::get_packet_number(reinterpret_cast<unsigned char *>(command)) != packet_number){
        printf("Bloque incorrecton");
        fclose(file);
        return;
    }

    fseek(file, 0L, SEEK_END );
    size_file=ftell(file);
    num_packets=size_file/512;
    mod_packets=size_file%512;
    rewind(file);
    data_file = static_cast<char *>(calloc(512, sizeof(char)));
    mod_packets ? last_data_file = static_cast<char *>(calloc(mod_packets, sizeof(char))): last_data_file = static_cast<char *>(calloc(
            1, sizeof(char)));
    if (data_file == NULL || last_data_file == NULL){
        printf("Error e lectura de archivo\n");
        fclose (file);
        return;
    }

    while(end!=2){
        packet_number++;
        if(packet_number<=num_packets){
            fread(data_file, 512,1,file);
            buffer = reinterpret_cast<char *>(helpers::prepare_data_to_send(packet_number,
                                                                            reinterpret_cast<BYTE *>(data_file)));
        } else{
            end=1;
            if (mod_packets){
                fread(last_data_file, mod_packets,1,file);
                data_to_send = mod_packets;
            } else{
                last_data_file[0]=0;
                data_to_send = 1;
            }
            buffer = reinterpret_cast<char *>(helpers::prepare_data_to_send(packet_number,
                                                                            reinterpret_cast<BYTE *>(last_data_file)));
        }

        sendto(socket,buffer,4+data_to_send,0, (struct sockaddr *)&clientaddr_in, addrlen);

        bytes_received = recvfrom (socket, command, 4,0,(struct sockaddr *)&clientaddr_in, &addrlen);

        if(bytes_received == -1){
            printf("%s","Error receiving ack");
            fclose(file);
            free(data_file);
            free(last_data_file);
            return;
        }

        type_option = helpers::get_packet_type(reinterpret_cast<BYTE *>(command));

        if (type_option == 5){
            printf("%s\n",command);
            fclose(file);
            free(data_file);
            free(last_data_file);
        }
        if(type_option != 4){
            printf("Se esperaba ack\n");
            fclose(file);
            free(data_file);
            free(last_data_file);
            return;
        }

        if(helpers::get_packet_number(reinterpret_cast<unsigned char *>(command)) != packet_number){
            printf("Bloque incorrecton");
            fclose(file);
            free(data_file);
            free(last_data_file);
            return;
        }

        if (end == 1) end =2;
    }


    printf(" success send file\n");

    fclose (file);
    free(data_file);
    free(last_data_file);
    return;

}
