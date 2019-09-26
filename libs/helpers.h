#ifndef TFTP_PG_HELPERS_H
#define TFTP_PG_HELPERS_H


class helpers {
public:
    helpers();
    static char ** get_my_ipv4();
    char ** get_my_ipv6();
    char * get_my_ip_public();

    static int get_packet_type(char* buffer);

    static char * get_filename(char* buffer);

    static int get_packet_number(unsigned char *packet);

    static char * get_data(char* buffer,int data_size);
};



#endif //TFTP_PG_HELPERS_H
