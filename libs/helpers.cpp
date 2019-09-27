#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <cstring>
#include "helpers.h"

char **helpers::get_my_ipv4() {
    struct ifaddrs *myaddrs, *ifa;
    void *in_addr;
    char buf[64];
    char *ips[10];

    if(getifaddrs(&myaddrs) != 0)
    {
        perror("getifaddrs");
        exit(1);
    }

    int i= 0;
    for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;
        if (!(ifa->ifa_flags & IFF_UP))
            continue;

        switch (ifa->ifa_addr->sa_family)
        {
            case AF_INET:
            {
                struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
                in_addr = &s4->sin_addr;
                break;
            }

            default:
                continue;
        }

        if (inet_ntop(ifa->ifa_addr->sa_family, in_addr, buf, sizeof(buf)))
        {
            ips[i] = static_cast<char *>(malloc(sizeof(buf)));
            strcpy(ips[i],buf);
        }
        i++;
    }

    freeifaddrs(myaddrs);
    return ips;
}

char **helpers::get_my_ipv6() {
    struct ifaddrs *myaddrs, *ifa;
    void *in_addr;
    char buf[64];
    char *ips[10];

    if(getifaddrs(&myaddrs) != 0)
    {
        perror("getifaddrs");
        exit(1);
    }

    int i= 0;
    for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;
        if (!(ifa->ifa_flags & IFF_UP))
            continue;

        switch (ifa->ifa_addr->sa_family)
        {
            case AF_INET6:
            {
                struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
                in_addr = &s4->sin_addr;
                break;
            }

            default:
                continue;
        }

        if (inet_ntop(ifa->ifa_addr->sa_family, in_addr, buf, sizeof(buf)))
        {
            ips[i] = static_cast<char *>(malloc(sizeof(buf)));
            strcpy(ips[i],buf);
        }
        i++;
    }

    freeifaddrs(myaddrs);
    return ips;
}

char *helpers::get_my_ip_public() {
    return NULL;
}


int helpers::get_packet_type(BYTE *buffer) {
    if(buffer == NULL)
    {
        printf("Error al obtener tipo de paquete: Paquete nulo.\n");
        return -1;
    }
    int packetType = buffer[0]*256+buffer[1];
    printf("PacketType: %d\n", packetType);
    return packetType;
}

char *helpers::get_filename(BYTE *buffer) {
    if(buffer == NULL)
    {
        printf("Null packet.\n");
        return NULL;
    }

    int i;
    int dataLength;
    for(dataLength = 0; buffer[2+dataLength]!=0; dataLength++){
        if(buffer[2+dataLength]==0)
            break;
    }
    char * filename = (char *) malloc((dataLength+1)* sizeof(char));
    for(i = 0; i<dataLength; i++)
        filename[i] = buffer[2+i];
    filename[dataLength] = '\0';
    return filename;
}


int helpers::get_packet_number(unsigned char *packet) {
    if(packet == NULL)
    {
        printf("Error al obtener el numero del paquete: Paquete nulo.\n");
        return -1;
    }

    int nPacket = packet[2]*256+packet[3];
    return nPacket;
}


char *helpers::get_data(char *buffer, int data_size) {
    if(buffer == NULL)
    {
        printf("Error al obtener el paquete: Paquete nulo.\n");
        return NULL;
    }

    char * data = (char *) malloc((data_size+1)* sizeof(char));

    memcpy(data, buffer+4, data_size);

    return data;
}


BYTE * helpers::WRQ_command(char *filename, char *mode) {
    int file_length = strlen(filename);
    int mode_length = strlen(mode);
    int i;

    BYTE * header = (BYTE *) calloc(2 + file_length + 1 + mode_length + 1, sizeof(BYTE));

    header[0] = 0;
    header[1] = 1;

    for(i=0; i < file_length; i++){
        header[2+i] = filename[i];
    }

    header[2 + file_length] = 0;

    for(i=0; i < mode_length; i++){
        header[2 + file_length + 1 + i] = mode[i];
    }

    header[2 + file_length + 1 + mode_length] = 0;

    return header;
}


BYTE *helpers::prepare_data_to_send(int Block, BYTE *data) {
    int dataLength = strlen(reinterpret_cast<const char *>(data));
    int i;

    BYTE * header = (BYTE *) calloc( 2+2+dataLength, sizeof(BYTE));

    header[0] = 0;
    header[1] = 3;
    if(Block >= 256*256){
        printf("Overflow number packet.\n");
        return NULL;
    }
    header[2] = Block/256;
    header[3] = Block%256;
    for(i = 0; i<dataLength; i++)
        header[4+i] = data[i];
    return header;
}

BYTE *helpers::make_ACK(int nBlock){

	BYTE * header = (BYTE *) calloc( 2+2, sizeof(BYTE));

// msg type
	header[0] = 0;
	header[1] = 4;

//nBloque
	if(nBlock >= 256*256){
		printf("N of block cant be fixed in 2 bytes.\n");
		return NULL;
	}
	header[2] = nBlock/256;
	header[3] = nBlock%256;


	for(int i=0; i<4; i++)
		printf(" %d ", header[i]);
	printf("\n");

	return header;
}
