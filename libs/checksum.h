#define CHECKSUM_HEADER
#include "sys/types.h"

//Add up checksum, return value will be filled in checksum filed in header
u_short add_checksum(u_short len_udp, int padding, const u_short *temp);