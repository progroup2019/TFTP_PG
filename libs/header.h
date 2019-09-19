#define RRQ "01" //Code for read request
#define WRQ "02" //Code for write
#define PCKT "03" //Code for data
#define ACK "04" //Code for acknoledge
#define ERR "05" //Code for error

class Header{
private:
	char *my_port; // Port to be opened on server
	int server_port; // Port users will be connecting to
	int max_buf_len; // Sockaddr, IPv4 or IPv6:
	int max_read_len; // Maximum data size that can be sent on one packet
	int max_filename_len; // Maximum length of file name supported
	int max_packets; // Maximum number of file packets
	int max_tries; // Maximum number of tries if packet times out
	int time_out; //Time out in seconds

public:
	Header(char* port, int actual_server_port, int buf_len, int read_len, int file_name_len, int packets, int tries, int timeout);

	~Header();

	void s_to_i(char *f, int n);

	char* make_rrq(char *file_name);

	char* make_wrq(char *file_name);

	char* make_data_pack(int block, char *data);

	char* make_ack(char* block);

	char* make_err(char *error_code, char* error_message);

	char*  get_port();

	int get_server_port();

	int get_max_buf_len();

	int get_max_read_len();

	int get_max_filename_len();

	int get_max_packets();

	int get_max_tries();

	int get_time_out();

	char * get_local_ip();
};