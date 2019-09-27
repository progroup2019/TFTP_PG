
#include "cstdio"

#include "libs/checksum.h"
#include "libs/server.h"


int main(int argc, char **argv)
{
    server *s = new server();

    s->init();
    return 0;
}

