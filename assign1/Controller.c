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
#define queue 1234

static int qid;
static bool send;



struct my_msg_st some_data;

void alarm_handler(){


send = true;
}


int server_starting(){
	//#if DEBUG_TRACE
	printf("%d :- Controller_starting()\n", getpid());
	//#endif 
	 printf("\n Type 'ctrl c' or 'command c' to quit\n");
	qid = msgget((key_t)queue, 0666| IPC_CREAT);
	if(qid == -1)return(0);
}

void server_ending(){
	//#if DEBUG_TRACE
	printf("%d :- server_ending()\n", getpid());
	//#endif 
	(void)msgctl(qid, IPC_RMID, 0);
	qid =-1;
	exit(EXIT_SUCCESS);
}


void alarm2(int sig){
	server_ending();
}

int main()
{	
     struct sigaction action;
	action.sa_handler = alarm_handler;
	sigaction(SIGALRM, &action, 0);

	 struct sigaction action2;
	action2.sa_handler = alarm2;
	sigemptyset(&action2.sa_mask);
	action2.sa_flags = 0;

	sigaction(SIGINT, &action2, 0);	

	int cloud_fifo;
    struct my_msg_st some_data;
   // int times_to_send;
    char client_fifo[256];
   
	int running =1;
    
    long int msg_to_receive = 0;
    long int msg_to_receive2 = 2;
		
	server_starting();
	 
	while(running) {
	 int pid;
         pid=fork();
	
         switch(pid)
	{
         case -1:	
            printf("\n unsuccessful fork: Error\n ");
            exit(1);
        
         case 0:
		msg_to_receive =3;
        	if (msgrcv(qid, (void *)&some_data, sizeof(some_data), msg_to_receive, IPC_NOWAIT) == -1) {
           		
       		 } 
		
		if(some_data.details.thresh > 1000) exit(EXIT_FAILURE);
		if(some_data.details.first_mess == true){
			printf("\n New Device detected ");
			printf("\n registering...");
			printf("\n Device name: %s\n ",some_data.details.name);
			printf("\n Device pid is %d\n ",some_data.details.pid);
			printf("\n Device threshhold is %d\n ",some_data.details.thresh);
		}
	
		if(some_data.details.thresh < some_data.details.val){
			some_data.my_msg_type = 2;
			alarm(1);
			pause();
			if (msgsnd(qid, (void *)&some_data, sizeof(some_data), 0) == -1) {
            fprintf(stderr, "msgsnd1 failed %d\n",errno);
		
            exit(EXIT_FAILURE);
        }
		}
		else if(some_data.details.val < some_data.details.thresh){ printf("\n Device %s current value is %d\n",some_data.details.name,some_data.details.val);}
		break;
	         
	default:
		while(send==true){
		 cloud_fifo = open(SERVER_FIFO_NAME, O_WRONLY);
 		  if (cloud_fifo == -1) {
        	fprintf(stderr, "Sorry, no server\n");
 	       exit(EXIT_FAILURE);}
		write(cloud_fifo,&some_data,sizeof(some_data));
		
		send = false;
		}
	
            break;
      
    }
	sleep(9);
}
    server_ending();
    exit(EXIT_SUCCESS);
}
      
