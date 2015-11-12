#include<stdbool.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/msg.h>

#define MAX_TEXT 512
#define queue 1234

static int qid;

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

int client_starting(){
	//#if DEBUG_TRACE
		printf("%d :- Device_starting\n",  getpid());
	//#endif

	qid = msgget((key_t)queue, 0666);
	if(qid == -1)return(0);

	return(1);
}

void client_ending(){
	//#if DEBUG_TRACE
		printf("%d :- Device_ending\n",  getpid());
	//#endif
	(void)msgctl(qid, IPC_RMID, 0);
	qid = -1;
	exit(EXIT_SUCCESS);
}
	

void alarm2(int sig){
	client_ending();
}

int main(int argc, char *argv[])
{
        struct my_msg_st some_data;
	//fgets(buffer, BUFSIZ, stdin);
	int arg;
	for(arg = 0; arg < argc; arg++){
		if(isdigit(argv[arg][0])){
			char *p;
			long conv = strtol(argv[arg], &p, 10);

			printf("threshold: %s\n", argv[arg]);
			some_data.details.thresh = conv;
		}
		else{
			printf("sensor %d: %s\n", arg, argv[arg]);
			strcpy(some_data.details.name, argv[arg]);
		}
	}
	long int msg_to_receive2 = 2;
	some_data.details.pid = getpid();
	
	struct sigaction action2;
	action2.sa_handler = alarm2;
	sigemptyset(&action2.sa_mask);
	action2.sa_flags = 0;

	sigaction(SIGINT, &action2, 0);	
    int running = 1;
    
    //int qid;
    
   client_starting();

	some_data.my_msg_type = 1;

	some_data.details.first_mess = true;
        int r = rand() % 20; 
	
	some_data.details.val = r;
	size_t siz = sizeof(some_data) - sizeof(some_data.my_msg_type);
	
	some_data.details.first_mess = true;
	some_data.my_msg_type = 3;
        if (msgsnd(qid, (void *)&some_data,  sizeof(some_data), 0) == -1) {
            fprintf(stderr, "msgsnd failed\n");
            exit(EXIT_FAILURE);
        }

	some_data.details.first_mess = false;

    while(running) 
	{	
        int r = rand() %50; 
	
	some_data.details.val = r;	
	
	some_data.my_msg_type = 3;

        if (msgsnd(qid, (void *)&some_data,  sizeof(some_data), 1) == -1) {
            fprintf(stderr, "msgsnd failed\n");
            exit(EXIT_FAILURE);
        	}

	//sleep(6);
	
	if (msgrcv(qid, (void *)&some_data, sizeof(some_data),
                   msg_to_receive2, IPC_NOWAIT) == -1) {
          // printf(s"devmsgrcv failed, ");
            //exit(EXIT_FAILURE);
        } else printf("\nThreshold Passed, A/C Off on Device: %s ID: %d\n",some_data.details.name,some_data.details.pid);	
	sleep(6);	
    }
	client_ending();

   exit(EXIT_SUCCESS);
}
