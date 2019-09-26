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
    return nullptr;
}


int helpers::get_packet_type(char *buffer) {
    if (strlen(buffer)){
        if (buffer[0] != '0') return -1;

        return  buffer[1]-'0';

    }else{
        perror("Paquete nulo");
        return -1;
    }
}

char *helpers::get_filename(char *buffer) {
    if (strlen(buffer)){
        int i;
        for (i = 2; i<strlen(buffer); i++){
            if(buffer[i] == 0) break;
        }

        char * filename = (char *) malloc((i-1)* sizeof(char));
        for(int j = 0; j<i; j++)
            filename[j] = buffer[2+j];
        filename[i] = '\0';
        return filename;

    } else {
        perror("Paquete nulo");
        return NULL;
    }
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
