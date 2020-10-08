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

void * testThreadOne(void* arg){
	printf("HELLO FROM THREAD 1\n");

	//THIS IS TO TEST YIELD
	/* int i = 0;
	while(i < 100000){
		printf("This is from thread one! BRUHHH \n");
		i++;
	}	
	mypthread_yield(NULL); */
	//////////////////////////

	//THIS IS TO TEST SCHEDULE SWAP
	while(1){
		//printf("HELLO FROM THREAD 1\n");
	}
	return NULL;
}

void * testThreadTwo(void* arg){
	int i = 0;
	printf("HELLO FROM THREAD 2\n");
	while(1){
		//printf("This is from thread two! \n");
		//i++;
	}
		
	//mypthread_yield(NULL);
	return NULL;
}

int main(int argc, char **argv) {

	/* Implement HERE */

	pthread_t thread_one;
	pthread_t thread_two;
	printf("Before thread \n");
	pthread_create(&thread_one, NULL, testThreadOne, NULL);
	// while(1){
	// 	printf("HELLO FROM MAIN\n");
	// }
	pthread_create(&thread_two, NULL, testThreadTwo, NULL);
	//pthread_join(thread_one, NULL);
	while(1){
		//printf("THIS IS STILL MAIN\n");
	}
	printf("after thread \n");

	sleep(10);
	//pthread_join(thread_two, NULL);
	// mypthread_yield();

 	return 0;
}
