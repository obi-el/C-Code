#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <sys/msg.h>
#include <stdbool.h>
#include "assignment1fifo.h"

static int running;
void alarm2(int sig){
	running=0;
}



int main(){

	struct sigaction action2;
	action2.sa_handler = alarm2;
	sigemptyset(&action2.sa_mask);
	action2.sa_flags = 0;

	sigaction(SIGINT, &action2, 0);		

	int cloud_fifo;
	struct my_msg_st some_data;
	int read_res;
	char client_fifo[256];
	char *tmp_char_ptr;
	
	printf("\nCloud Process Starting...\n");
	    mkfifo(SERVER_FIFO_NAME, 0777);
    cloud_fifo = open(SERVER_FIFO_NAME, O_RDONLY);
    if (cloud_fifo == -1) {
        fprintf(stderr, "Server fifo failure\n");
        exit(EXIT_FAILURE);
    }
	 running=1;
    sleep(12); /* lets clients queue for demo purposes */
while(running =1){
    do {
        read_res = read(cloud_fifo, &some_data, sizeof(some_data));
        if (read_res > 0) {
            printf("Device Data Receieved from: %s ID: %d\n",some_data.details.name,some_data.details.pid);}
    } while (read_res > 0);
}


    close(cloud_fifo);
    unlink(SERVER_FIFO_NAME);
    exit(EXIT_SUCCESS);
}	




