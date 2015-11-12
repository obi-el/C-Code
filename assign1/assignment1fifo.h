
#include <fcntl.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/types.h>
#include<sys/stat.h>

#define SERVER_FIFO_NAME "/tmp/serv_fifo"
#define CLIENT_FIFO_NAME "/tmp/cli_%d_fifo"

#define BUFFER_SIZE 20

struct my_msg_st {
    long int my_msg_type;
    struct details{
	char name[25];
	bool first_mess;
	int thresh;
	int val;
	pid_t pid;
	//char device_type;
	}details;
};

