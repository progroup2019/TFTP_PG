#ifndef TFTP_PG_HELPERS_H
#define TFTP_PG_HELPERS_H


class helpers {
public:
    helpers();
    static char ** get_my_ipv4();
    char ** get_my_ipv6();
    char * get_my_ip_public();
};



#endif //TFTP_PG_HELPERS_H