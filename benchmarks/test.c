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
int* ptr;
int counter;
pthread_mutex_t lock;

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
	  *ptr = 100;

	  pthread_mutex_lock(&lock);
	  counter++;
	  //pthread_mutex_unlock(&lock);

	  while(i < 1000000){
	    //i++;
	  }
	  

	  mypthread_exit((void*)ptr);
	  //}
	  
	return NULL;
}

void * testThreadTwo(void* arg){
	int i = 0;
	printf("HELLO FROM THREAD 2\n");

	pthread_mutex_lock(&lock);
	counter++;
	//pthread_mutex_unlock(&lock);
	while(i < 100000){
		//printf("This is from thread two! \n");
		//i++;
	}
		
	mypthread_exit(NULL);
	//mypthread_yield(NULL);
	return NULL;
}

int main(int argc, char **argv) {

	/* Implement HERE */
  ptr = malloc(sizeof(int*));
  *ptr = 500;
  ret1 = 200;

  counter = 0;

	pthread_t thread_one;
	pthread_t thread_two;

	pthread_mutex_init(&lock, NULL);


	printf("Before thread \n");
	pthread_create(&thread_one, NULL, &testThreadOne, NULL);
	
	pthread_create(&thread_two, NULL, testThreadTwo, NULL);
	pthread_join(thread_one, NULL);
	pthread_join(thread_two,NULL);
        int i =1;
	
	printf("THIS IS STILL MAIN\n");
	//while(1);
	void *retValue; 


	// mypthread_join(thread_one, (void**)ptr);
	// printf("Return value from thread1 is %d\n", (*ptr)); 
	mypthread_join(thread_two, NULL);

	printf("From main, lock status: %d\n", lock.locked);
	printf("counter at end of main: %d\n", counter);
	//pthread_mutex_lock(&lock);
	//}
	//printf("after thread \n");

	//sleep(10);
	//	mypthread_join(thread_one, (void**)&(ptr[0]));
	//printf("Return value from thread1 is %d\n", *ptr[0]);
	// mypthread_yield();

 	return 0;
}
