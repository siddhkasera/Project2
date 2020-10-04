#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../mypthread.h"

/* A scratch program template on which to call and
 * test mypthread library functions as you implement
 * them.
 *
 * You can modify and use this program as much as possible.
 * This will not be graded.
 */

void * testThread(void* arg){
	sleep(1);
	printf("This is from thread! \n");
	return NULL;
}
int main(int argc, char **argv) {

	/* Implement HERE */

	pthread_t thread_id;
	printf("Before thread \n");
	pthread_create(&thread_id, NULL, testThread, NULL);
	pthread_join(thread_id, NULL);
	printf("after thread \n");

	return 0;
}
