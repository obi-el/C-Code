
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

/*
* Created 3 semaphores one for mutual exclusion(mutex set to 1) one to keep track of empty buffers(empty set to k ) and the
* last keeps track of if there is an available buffer to be read(available set to 0)  
* 
*/

static int set_mutex(void);
static int set_available(void);
static int set_empty(void);
static void del_semvalue(void);
static int mutex_p(void);
static int mutex_v(void);
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

	//get the semaphores	

    sem_id = semget((key_t)1233, 1, 0666 | IPC_CREAT);
    sem_id2 = semget((key_t)1235, 1, 0666 | IPC_CREAT);
    sem_id3 = semget((key_t)1236, 1, 0666 | IPC_CREAT);

/*
*sets the semaphores to the required values if it fails program ends
*/
    
	if (!set_mutex()| !set_available()| !set_empty()) {
            fprintf(stderr, "Failed to initialize semaphore\n");
            exit(EXIT_FAILURE);
        }

	//get shared memory

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
 Reads in from the input file test4k.txt into the buffer ibuffer string of size BUFSIZ, then the 
 loop continually copies text from ibuffer into the buffers in shared memory which take 128 bytes of char until a value less than 128 is written which means approached end of text then it sleeps and deletes the semaphores */

    shared_stuff = (struct shared_use_st *)shared_memory;
    //shared_stuff->written_by_you = 0;

    char ibuffer[BUFSIZ];   // input buffer 
 
    int in; // variable to hold reference to input file
    int index=0; // current index of element in array of structs in shared memory 
    int len;  // length read 
     int tot = 0; // Total length read

    in = open("test4k.txt",O_RDONLY);
   
	len = read(in,ibuffer, sizeof(ibuffer));
    while(running){
	
	for(int i = 0; i<= len; i+=128){
		sleep(2);
		if((len-i)<128){

		//if value written is less than 128  delete semaphores


		shared_stuff->msgs[index].written = len-i;
		memcpy(shared_stuff->msgs[index].some_text, ibuffer + (len-i), (len-i));
		tot+= shared_stuff->msgs[index].written;
		printf("Producer: bytes written val %d\n", shared_stuff->msgs[index].written);

		printf("Producer: Total bytes written val %d\n", tot);		

		sleep(10);
		del_semvalue();
		exit(EXIT_SUCCESS);
		
		}
		else{
		empty_p();	
		mutex_p();

 		shared_stuff->msgs[index].written = 128;
		
		printf("Producer: bytes written val %d\n", shared_stuff->msgs[index].written);
		memcpy(shared_stuff->msgs[index].some_text,ibuffer + i,128);		
		tot+= shared_stuff->msgs[index].written;
		
		index = (index+1)%k;  
		mutex_v();
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

	

    exit(EXIT_SUCCESS);
}

static int set_mutex(void)
{
    union semun sem_union;

    sem_union.val = 1;
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1) return(0);
    
    return(1);
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
    
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore mutex\n");

    if (semctl(sem_id2, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore available\n");

    if (semctl(sem_id3, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore empty\n");
}

/* mutex_p changes the semaphore responsible for mutual exclusion to by -1 (waiting). */

static int mutex_p(void)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = 0;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "mutex_p prod failed\n");
        return(0);
    }
    return(1);
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



/* mutex_v is similar except for setting the sem_op part of the sembuf structure to 1,
 so that the mutual exclusion semaphore becomes available. */

static int mutex_v(void)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = 1; /* V() */
    sem_b.sem_flg = 0;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "mutex_v prod failed\n");
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


