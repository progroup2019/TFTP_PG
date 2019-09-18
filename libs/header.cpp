#include "header.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// converts block number to length-2 string
void Header::s_to_i(char *f, int n){
	if(n==0){
		f[0] = '0', f[1] = '0', f[2] = '\0';
	} else if(n%10 > 0 && n/10 == 0){
		char c = n+'0';
		f[0] = '0', f[1] = c, f[2] = '\0';
	} else if(n%100 > 0 && n/100 == 0){
		char c2 = (n%10)+'0';
		char c1 = (n/10)+'0';
		f[0] = c1, f[1] = c2, f[2] = '\0';
	} else {
		f[0] = '9', f[1] = '9', f[2] = '\0';
	}
}

// makes RRQ packet
char* Header::make_rrq(char *file_name){
	char *packet;
	packet = malloc(2+strlen(file_name));
	memset(packet, 0, sizeof packet);
	strcat(packet, RRQ); //option code
	strcat(packet, file_name);
	return packet;
}

// makes WRQ packet
char* Header::make_wrq(char *file_name){
	char *packet;
	packet = malloc(2+strlen(file_name));
	memset(packet, 0, sizeof packet);
	strcat(packet, WRQ); //option code
	strcat(packet, file_name);
	return packet;
}

// makes data packet
char* Header::make_data_pack(int block, char *data){
	char *packet;
	char temp[3];
	s_to_i(temp, block);
	packet = malloc(4+strlen(data));
	memset(packet, 0, sizeof packet);
	strcat(packet, PCKT); //option code
	strcat(packet, temp);
	strcat(packet, data);
	return packet;
}

// makes ACK packet
char* Header::make_ack(char* block){
	char *packet;
	packet = malloc(2+strlen(block));
	memset(packet, 0, sizeof packet);
	strcat(packet, ACK); //option code
	strcat(packet, block);
	return packet;
}

// makes ERR packet
char* Header::make_err(char *error_code, char* error_message){
	char *packet;
	packet = malloc(4+strlen(error_message));
	memset(packet, 0, sizeof packet);
	strcat(packet, ERR); //option code
	strcat(packet, error_code);
	strcat(packet, error_message);
	return packet;
}

char * Header::get_port(){
	return my_port;
}

char * Header::get_server_port(){
	return server_port;
}

int Header::get_max_buf_len(){
	return max_buf_len;
}

int Header::get_max_read_len(){
	return max_read_len;
}

int Header::get_max_filename_len(){
	return max_filename_len;
}

int Header::get_max_packets(){
	return max_packets;
}

int Header::get_max_tries(){
	return max_tries;
}

int Header::get_time_out(){
	return time_out;
}

// Constructor function
Header::Header(char* port, char* actual_server_port, int buf_len, int read_len, int file_name_len, int packets, int tries, int timeout){
	strcpy(my_port, port);
	strcpy(server_port, actual_server_port);
	max_buf_len = buf_len;
	max_read_len = read_len;
	max_filename_len = file_name_len;
	max_packets = packets;
	max_tries = tries;
	time_out = timeout;
}

// Destructor function
Header::~Header() {

}