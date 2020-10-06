// File:	mypthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "mypthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE

#define STACK_SIZE SIGSTKSZ

mypthread_t numOfThreads = 1;
int curr_id = 1;

void new_node(){
  tcb new_thread;
  temp = (struct node*) malloc (1 * sizeof(struct node));
  temp->next = NULL;
}

void enqueue ( tcb * new_thread){
	printf("Adding thread %d to list: \n", new_thread->thread_id);
  if(head == NULL){
    new_node();
    head = temp;
	tail = temp;
	head->next = head;
	head->prev = head;
  }else{
    new_node();
    prev->next = temp;
	temp->prev = tail;
    tail = temp;
	temp->next = head;
	head->prev = tail;
   }
  temp->n = new_thread;

  printf("the head: %d \n", head->n->thread_id);

	//Print the current list
	printf("Printing current thread list: \n");
  struct node* ptr = head;
  do
  {
	  printf("thread %d \n", ptr->n->thread_id);
		ptr = ptr->next;
  } while (ptr != head);
  
 
}

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr,
                      void *(*function)(void*), void * arg) {
       // create Thread Control Block
       // create and initialize the context of this thread
       // allocate space of stack for this thread to run
       // after everything is all set, push this thread int
       // YOUR CODE HERE

       printf("In mypthread_create: \n");
       // Allocate for tcb
	   printf("Allocating for tcb: \n");
       tcb* new_thread = (tcb*) malloc(sizeof(tcb*));
	   printf("tcb allocation complete \n");
        *thread = numOfThreads++;
        new_thread->thread_id = *thread;

		// Allocate for context
		printf("Allocating for context: \n");
        ucontext_t* ctx = (ucontext_t*) malloc(sizeof(ucontext_t));
        new_thread->thread_ctx = ctx;
		printf("context allocation complete \n");
		if(getcontext(ctx) == -1){
            printf("Error getting contxt\n");
        }

		// Allocate the stack
		printf("Allocating for stack: \n");
        (new_thread)->thread_ctx->uc_stack.ss_sp = (char*) malloc(sizeof(char) * STACK_SIZE);
		(new_thread)->thread_ctx->uc_stack.ss_size = STACK_SIZE;
		printf("stack allocation complete \n");

		(new_thread)->thread_status = READY;
		// TODO: Add Prioirity
		
	 	makecontext(new_thread->thread_ctx,(void (*)()) function, 1, arg);

		// Add to list..?
	 	enqueue(new_thread);

    return 0;
};

tcb * searchBlock(int threadId){
  printf("Searching for block 1\n");
  temp = head;
  while(temp !=NULL){
    if(temp->n->thread_id == threadId){
      printf("Found the thread: %d\n", temp->n->thread_id);
      return temp->n;
    }else{
      temp = temp->next;
    }
  }
  printf("Thread not found\n");
  exit(0);
}

tcb * searchNextBlock(int threadId){
  printf("Searching for the next block\n");
  temp = head;
  while(temp !=NULL){
    if(temp->n->thread_id == threadId){
      printf("Found a thread that is not what you were looking for: %d\n", temp->n->thread_id);
      break;
    }else{
      temp = temp->next;
    }
  }
    if(temp != NULL){
      while(temp->next !=NULL){
	if(temp->n->thread_status == 0){ //checking the status
	  printf("Found the next thread: %d\n", temp->n->thread_id);
	  return temp->n;
	}else{
	  temp = temp ->next;
	}
      }
    
    }
    printf("No ready threads\n");
    exit(0);

}


/* give CPU possession to other user-level threads voluntarily */

int mypthread_yield() {

  printf("Start of the yield method\n");
  tcb * newBlock = searchBlock(curr_id);
  printf("New block found\n");
  newBlock->thread_status = READY;
  
  int newBlockContext = getcontext(newBlock->thread_ctx);
  printf("the new context is : %d\n", newBlockContext);

  tcb * next_block = searchNextBlock(newBlock->thread_id);
  printf("the new next block id is:%d\n", newBlock->thread_id);
  swapcontext(newBlock->thread_ctx, next_block->thread_ctx);
  printf("Context successfully swapped\n");
  // change thread state from Running to Ready
	// save context of this thread to its thread control block
	// wwitch from thread context to scheduler context

	// YOUR CODE HERE
	return 0;
};

/* terminate a thread */
void mypthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread

	// YOUR CODE HERE
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {

	// wait for a specific thread to terminate
	// de-allocate any dynamic memory created by the joining thread

	// YOUR CODE HERE
	return 0;
};

/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex,
                          const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex

	// YOUR CODE HERE
	return 0;
};

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //
        // context switch to the scheduler thread

        // YOUR CODE HERE
        return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.

	// YOUR CODE HERE
	return 0;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in mypthread_mutex_init

	return 0;
};

/* scheduler */
static void schedule() {
	// Every time when timer interrup happens, your thread library
	// should be contexted switched from thread context to this
	// schedule function

	// Invoke different actual scheduling algorithms
	// according to policy (STCF or MLFQ)

	// if (sched == STCF)
	//		sched_stcf();
	// else if (sched == MLFQ)
	// 		sched_mlfq();

	// YOUR CODE HERE

// schedule policy
#ifndef MLFQ
	// Choose STCF
#else
	// Choose MLFQ
#endif

}

/* Preemptive SJF (STCF) scheduling algorithm */
static void sched_stcf() {
	// Your own implementation of STCF
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

// Feel free to add any other functions you need

// YOUR CODE HERE
