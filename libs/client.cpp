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
        fprintf(stderr, "%s: Does'nt possible resolving IP %s\n",
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

    if (strcmp(argv[2], "POST") == 0)
    {
        send_file(sock_client,argv[3],"octec",serv_addr);
    } else if (strcmp(argv[2], "GET") == 0) {
        receiving_file(sock_client,argv[3],"octec",serv_addr);
    }

}

void client::send_file(int socket, char *filename, char *mode, struct sockaddr_in clientaddr_in) {

    printf("Sending file %s \n",filename);
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
        printf("File does not exist %s\n", filename);
        return;
    }

    buffer = reinterpret_cast<char *>(helpers::WRQ_command(filename, mode));


    sendto(socket,buffer,2+strlen(filename)+1+strlen(mode)+1,0,(struct sockaddr *)&clientaddr_in, addrlen);

    bytes_received = recvfrom(socket,command,4,0,(struct sockaddr *)&clientaddr_in, &addrlen);

    if(bytes_received == -1){
        printf("Error receiving message\n");
    }

    type_option = helpers::get_packet_type(reinterpret_cast<BYTE *>(command));

    if (type_option == 5){
        printf("%s\n",command);
        fclose(file);
    }
    if(type_option != 4){
        printf("We have waiting for ACK\n");
        fclose(file);
        return;
    }

    if(helpers::get_packet_number(reinterpret_cast<unsigned char *>(command)) != packet_number){
        printf("Incorrect block\n");
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
        printf("Error reading file\n");
        fclose (file);
        return;
    }

    while(end!=2){
        packet_number++;
        if(packet_number<=num_packets){
            fread(data_file, PACKET_SIZE,1,file);
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
        printf("Sending packet %d \n",packet_number);
        sendto(socket,buffer,4+data_to_send,0, (struct sockaddr *)&clientaddr_in, addrlen);

        bytes_received = recvfrom (socket, command, 4,0,(struct sockaddr *)&clientaddr_in, &addrlen);


        printf("Receiving ACK\n");

        if(bytes_received == -1){
            printf("%s","Error receiving ACK");
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
            printf("We have waiting ACK\n");
            fclose(file);
            free(data_file);
            free(last_data_file);
            return;
        }
        if(helpers::get_packet_number(reinterpret_cast<unsigned char *>(command)) != packet_number){
            printf("Incorrect block %d\n",helpers::get_packet_number(reinterpret_cast<unsigned char *>(command)));
            fclose(file);
            free(data_file);
            free(last_data_file);
            return;
        }

        if (end == 1) end =2;
    }


    printf("Success send file\n");

    fclose (file);
    free(data_file);
    free(last_data_file);
    return;

}

void client::receiving_file(int socket, char *filename, char *mode, struct sockaddr_in clientaddr_in) {
    int packet_number= 0, end= 0;
    int bytes_read, type_option;
    printf("Receiving file :%s\n",filename);
    char *buffer;

    char data_file[PACKET_SIZE+4];

    FILE *file;

    socklen_t addrlen;
    addrlen = sizeof(struct sockaddr_in);


    socklen_t len;
    len = sizeof(len);
    if (getsockname(socket, (struct sockaddr *)&my_addr, &len) == -1)
        perror("getsockname");


    int port_number = ntohs(my_addr.sin_port);

    file = fopen(filename,"rb");

    if (file !=NULL){
        printf("File %s exists\n",filename);
        fclose(file);
        return;
    }

    file =fopen(filename,"wb");

    buffer = reinterpret_cast<char *>(helpers::RRQ_command(filename, mode));
    printf("Sending RRQ request");
    sendto (socket, buffer, 2+strlen(filename)+1+strlen(mode)+1,0, (struct sockaddr *)&clientaddr_in, addrlen);

    while (end != 2){
        packet_number++;

        bytes_read = recvfrom (socket, data_file, PACKET_SIZE+4,0,(struct sockaddr *)&clientaddr_in, &addrlen);

        if(bytes_read == -1){
            printf("Error receiving data\n");
            return;
        }

        type_option = helpers::get_packet_type(reinterpret_cast<BYTE *>(data_file));

        if(type_option == 5){
            fclose(file);
            perror(helpers::get_data(reinterpret_cast<BYTE *>(data_file), bytes_read));
            return;
        }

        if (type_option != 3){
            printf("Invalid Packet type\n");
            fclose(file);
            return;
        }

        if (helpers::get_packet_number(reinterpret_cast<unsigned char *>(data_file)) != packet_number){
            printf("Incorrect block %d\n",helpers::get_packet_number(reinterpret_cast<unsigned char *>(data_file)));
            fclose(file);
            return;
        }

        fwrite(helpers::get_data(reinterpret_cast<BYTE *>(data_file), bytes_read-4), bytes_read-4, 1, file);

        buffer = reinterpret_cast<char *>(helpers::ACK(packet_number));
        printf("Receive 04 with block number %d\n",helpers::get_packet_number(reinterpret_cast<unsigned char *>(data_file)));
        sendto (socket, buffer, 4,0, (struct sockaddr *)&clientaddr_in, addrlen);
        if (bytes_read-4 < PACKET_SIZE) end = 2;
    }

    fclose(file);
    printf("Success received file\n");

    return;
}
