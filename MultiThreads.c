#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 6
int running;

void *thread_functionprod();
void *thread_functioncon(void *arg);

struct sh_buffers{
	struct buffs{
		int bufno;
		int data;
	}buffers[5];
}shared;

int main() {
    int res;
    pthread_t a_thread[NUM_THREADS];
    pthread_t prod_thread;
    void *thread_result;
    int lots_of_threads;

    for(int i = 0; i <= NUM_THREADS; i++){
        shared.buffers[i].bufno = i;
    }
    
    res = pthread_create( &prod_thread ,NULL, thread_functionprod, NULL);
    if (res != 0) {
        perror("Attribute creation failed");
        exit(EXIT_FAILURE);
    }

	
    for(lots_of_threads = 0; lots_of_threads < NUM_THREADS; lots_of_threads++) {

        res = pthread_create(&(a_thread[lots_of_threads]), NULL, thread_functioncon, (void *)&lots_of_threads);
        if (res != 0) {
            perror("Thread creation failed\n");
            exit(EXIT_FAILURE);
        }
        sleep(1);
    }

     printf("Waiting for threads to finish...\n");
     sleep(3);
     printf("All done\n");
    exit(EXIT_SUCCESS);
    }

void *thread_functionprod() {
    
    int rand_num;

    printf("Producer is running.\n");
    
    for (int i = 0; i <= NUM_THREADS; i++){
	 rand_num= rand()% 50;
         shared.buffers[i].data = rand_num;
    }
    sleep(2);
    printf("Bye from Producer\n");
    pthread_exit(NULL);
}

void *thread_functioncon(void *arg) {
    int my_number = *(int *)arg;
    int rand_num;
    //if(my_number == sh_buffers.buffers[i].bufno){
    printf("Consumer %d val is: %d\n",my_number, shared.buffers[my_number].data);
    rand_num=1+(int)(9.0*rand()/(RAND_MAX+1.0));
    sleep(rand_num);
    printf("Bye from Consumer %d\n", my_number);
    pthread_exit(NULL);
}
