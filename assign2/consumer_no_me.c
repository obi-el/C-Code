/* The second program is the producer and allows us to enter data for consumers.
 It's very similar to shm1.c and looks like this. */

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "shm_com.h"
#include "semun.h"

static int set_available(void);
static int set_empty(void);
static void del_semvalue(void);
static int available_p(void);
static int available_v(void);
static int empty_p(void);
static int empty_v(void);


static int sem_id;
static int sem_id2;
static int sem_id3;


int main()
{
	
    int running = 1;
    void *shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;
    char buffer[BUFSIZ];
    int shmid;

     
    sem_id2 = semget((key_t)1235, 1, 0666 | IPC_CREAT);
    sem_id3 = semget((key_t)1236, 1, 0666 | IPC_CREAT);

    shmid = shmget((key_t)1231, (sizeof(struct shared_use_st)- sizeof(int)), 0666 | IPC_CREAT);

    if (shmid == -1) {
        fprintf(stderr, "shmget failed con\n");
      exit(EXIT_FAILURE);
    }

    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Memory attached at %X\n", (void *)shared_memory);
	int index =0;
    shared_stuff = (struct shared_use_st *)shared_memory;
   	
    int out;  // variable to hold reference to output document
    int len; // length written each time
    int tot =0;	//total length written
    
    out = open("outp.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    while(running){
	
	if(shared_stuff->msgs[index].written == 0) exit(EXIT_SUCCESS);
	else if(shared_stuff->msgs[index].written < 128){
	len = write(out,shared_stuff->msgs[index].some_text,shared_stuff->msgs[index].written);
	tot += len;
	printf("Consumer: Total Bytes written is %d\n", tot);
	printf("Consumer: length written is %d, mem written in struct is %d END OF INPUT\n", len, shared_stuff->msgs[index].written);
	
	 exit(EXIT_SUCCESS);
	}
        available_p();
	
   	len = write(out,shared_stuff->msgs[index].some_text,shared_stuff->msgs[index].written);
	
	if(len != shared_stuff->msgs[index].written) printf("error, bytes compared not the same\n");
	
	printf("Consumer: Bytes written is %d\n", len);
	tot+=len;
	index = (index +1)% 100;
	empty_v();
	
	
	}


    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

static int set_available(void)
{
    union semun sem_union;

    sem_union.val = 0;
    if (semctl(sem_id2, 0, SETVAL, sem_union) == -1) return(0);
    
    return(1);
}

static int set_empty(void)
{
    union semun sem_union;

    sem_union.val = k;
    
    if (semctl(sem_id3, 0, SETVAL, sem_union) == -1) return(0);
    return(1);
}

/* The del_semvalue function has almost the same form, except the call to semctl uses
 the command IPC_RMID to remove the semaphore's ID. */

static void del_semvalue(void)
{
    union semun sem_union;

    if (semctl(sem_id2, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore available\n");

    if (semctl(sem_id3, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore empty\n");
}

/* available_p changes the semaphore responsible for keeping track of files available to be read from by -1 (waiting). */

static int available_p(void)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = 0;
    if (semop(sem_id2, &sem_b, 1) == -1) {
        fprintf(stderr, "available_p failed\n");
        return(0);
    }
    return(1);
}


/* empty_p changes the semaphore responsible for keeping track of empty spaces by -1 (waiting). */

static int empty_p(void)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = 0;
    if (semop(sem_id3, &sem_b, 1) == -1) {
        fprintf(stderr, "empty_p failed\n");
        return(0);
    }
    return(1);
}

static int available_v(void)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = 1; /* V() */
    sem_b.sem_flg = 0;
    if (semop(sem_id2, &sem_b, 1) == -1) {
        fprintf(stderr, "available_v failed\n");
        return(0);
    }
    return(1);
}

static int empty_v(void)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = 1; /* V() */
    sem_b.sem_flg = 0;
    if (semop(sem_id3, &sem_b, 1) == -1) {
        fprintf(stderr, "empty_v failed\n");
        return(0);
    }
    return(1);
}




