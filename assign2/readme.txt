Authors: Craig Isesele & Obinna Elobi
Contact: obinnaelobi@cmail.carleton.ca, craigisesele@cmail.carleton.ca

HOW TO RUN
-in the terminal type 'make' to compile all the files
-if running in one terminal ./producer & ./consumer
-if running in multiple terminals ensure to run consumers first so that producer doesnt write into all the buffers and delete the semaphores since input size of text which is a 4k file is less than 12800(size of buffers 128 x 100 buffers)  and give semaphore errors at the end.


Bugs
-When multiple consumers are run the producer deletes the semaphores before the consumers finish hence we get semaphore errors but it still counts and reads correct number of bytes


Details
-The 'producer_no_me' and 'consumer_no_me' files are simply the producer and consumer
algorithms without semaphore s implemented. Therefore the mutual exclusion semaphore was ignored
It ran successfully without the semaphore with one producer and consumer
-For the other tests the time the producer waits for the consumer was reduced to 4 from 10
and the sleep for each iteration was removed. Therefore the program runs faster. Producer notably
ran faster than consumer
-Performance Evaluation file contains the charts for the results
-128 was used for the 'file size' and 'number of buffer' tests
-'6k.txt' was used for all other tests besides the file_size test

Buffer Number Test
-Runtime speed is directly proportional to number of buffers
-Above 100 buffers the program encounters errno 22 (EINVAL) because size was greater than SHMMAX, as seen http://linux.die.net/man/2/shmget

Buffer Size Test
-This test failed for cases above 128 with errno 22 (EINVAL) as size was greater than SHMMAX, as seen http://linux.die.net/man/2/shmget

Semaphore comparison
-The producer and consumer ran at 11088 microseconds and 31370 microseconds without semaphores
-The producer and consumer ran at 52393 microseconds and 54077 microseconds with semaphores
-In conclusion the algorithm is much quicker when semaphores aren't used


