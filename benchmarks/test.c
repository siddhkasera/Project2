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

int ret1;

void * testThreadOne(void* arg){
  //printf("HELLO FROM THREAD 1\n");

	//THIS IS TO TEST YIELD
	/* int i = 0;
	while(i < 100000){
		printf("This is from thread one! BRUHHH \n");
		i++;
	}	
	mypthread_yield(NULL); */
	//////////////////////////

	//THIS IS TO TEST SCHEDULE SWAP
	//while(1){
		//printf("HELLO FROM THREAD 1\n");
	//}
	//THIS IS FOR JOIN AND DELETE
  
	int i =1;
        //if( i <=1){
	  printf("HELLO FROM THREAD 1\n");
	  ret1 = 100;
	  
	  mypthread_exit(&ret1);
	  //}
	  
	return NULL;
}
/*
void * testThreadTwo(void* arg){
	int i = 0;
	printf("HELLO FROM THREAD 2\n");
	while(1){
		//printf("This is from thread two! \n");
		//i++;
	}
		
	//mypthread_yield(NULL);
	return NULL
}
*/
int main(int argc, char **argv) {

	/* Implement HERE */
  int * ptr;
  ret1 = 200;
	pthread_t thread_one;
	pthread_t thread_two;
	printf("Before thread \n");
	pthread_create(&thread_one, NULL, &testThreadOne, NULL);
	// while(1){
	
	//}
	//pthread_create(&thread_two, NULL, testThreadTwo, NULL);
	//pthread_join(thread_one, NULL);
        int i =1;
	//while(i <=1){
	printf("THIS IS STILL MAIN\n");
	mypthread_join(thread_one, (void**)&(ptr));
	printf("Return value from thread1 is %d\n", ret1); 
	//}
	//printf("after thread \n");

	//sleep(10);
	//	mypthread_join(thread_one, (void**)&(ptr[0]));
	//printf("Return value from thread1 is %d\n", *ptr[0]);
	// mypthread_yield();

 	return 0;
}
