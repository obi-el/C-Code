/* A common header file to describe the shared memory we wish to pass about. */

#define TEXT_SZ 128
 int k = 100;

struct shared_use_st {
   // int id;
    int written_by_you;
    struct msg{
		int written;
		char some_text[TEXT_SZ];
	}msgs[100];
};
