#ifndef TFTP_PG_HELPERS_H
#define TFTP_PG_HELPERS_H
#define PACKET_SIZE 512
typedef unsigned char BYTE;

class helpers {
public:
    helpers();
    static char ** get_my_ipv4();
    char ** get_my_ipv6();
    char * get_my_ip_public();

    static int get_packet_type(BYTE * buffer);

    static char * get_filename(BYTE* buffer);

    static int get_packet_number(unsigned char *packet);

    static char * get_data(BYTE* buffer,int data_size);

    static BYTE * WRQ_command(char* filename, char* mode);

    static BYTE * prepare_data_to_send(int Block,BYTE * data);

    static BYTE * RRQ_command(char* filename, char * mode);

    static BYTE * ACK(int block);

    static void ACK_ERROR(int socket,struct sockaddr_in clientaddr_in, int code_error, char * err_msg);
};



#endif //TFTP_PG_HELPERS_H
