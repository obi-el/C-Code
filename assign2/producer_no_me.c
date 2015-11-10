/* Our first program is a consumer. After the headers the shared memory segment
 (the size of our shared memory structure) is created with a call to shmget,
 with the IPC_CREAT bit specified. */

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    int shmid;

    srand((unsigned int)getpid());

    sem_id = semget((key_t)1233, 1, 0666 | IPC_CREAT);
    sem_id2 = semget((key_t)1235, 1, 0666 | IPC_CREAT);
    sem_id3 = semget((key_t)1236, 1, 0666 | IPC_CREAT);
    
	if (!set_available()| !set_empty()) {
            fprintf(stderr, "Failed to initialize semaphore\n");
            exit(EXIT_FAILURE);
        }

    shmid = shmget((key_t)1231, (sizeof(struct shared_use_st)- sizeof(int)) , 0666 | IPC_CREAT);

    if (shmid == -1) {
        fprintf(stderr, "shmget failed prod\n");
        exit(EXIT_FAILURE);
    }

/* We now make the shared memory accessible to the program. */

    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Memory attached at %X\n", (void *)shared_memory);

/* The next portion of the program assigns the shared_memory segment to shared_stuff,
 which then prints out any text in written_by_you. The loop continues until end is found
 in written_by_you. The call to sleep forces the consumer to sit in its critical section,
 which makes the producer wait. */

    shared_stuff = (struct shared_use_st *)shared_memory;
    //shared_stuff->written_by_you = 0;

    char ibuffer[BUFSIZ];   // input buffer 
 
    int in; // variable to hold reference to input file
    int index=0; // current index of element in array of structs in shared memory 
    int len;  // length read 
     int tot = 0; // Total length read

    in = open("6k.txt",O_RDONLY);
   
	len = read(in,ibuffer, sizeof(ibuffer));
    while(running){
	
	for(int i = 0; i<= len; i+=128){
		//sleep(2);
		if((len-i)<128){
		
		shared_stuff->msgs[index].written = len-i;
		memcpy(shared_stuff->msgs[index].some_text, ibuffer + (len-i), (len-i));
		tot+= shared_stuff->msgs[index].written;
		printf("Producer: bytes written val %d\n", shared_stuff->msgs[index].written);

		printf("Producer: Total bytes written val %d\n", tot);		

		sleep(4);
		del_semvalue();
		exit(EXIT_SUCCESS);
		
		}
		else{
		empty_p();	
		
 		shared_stuff->msgs[index].written = 128;
		//printf("writte\n");
		printf("Producer: bytes written val %d\n", shared_stuff->msgs[index].written);
		memcpy(shared_stuff->msgs[index].some_text,ibuffer + i,128);		
		tot+= shared_stuff->msgs[index].written;
		
		index = (index+1)%k;  

		available_v();
		
		}
	}
   }
  

/* Lastly, the shared memory is detached and then deleted. */

    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

	//del_semvalue();

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
        fprintf(stderr, "available_p prod failed\n");
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
        fprintf(stderr, "empty_p prod failed\n");
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
        fprintf(stderr, "available_v prod failed\n");
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
        fprintf(stderr, "empty_v prod failed\n");
        return(0);
    }
    return(1);
}


