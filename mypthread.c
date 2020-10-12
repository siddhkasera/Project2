#include "mypthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>


#define STACK_SIZE SIGSTKSZ

mypthread_t numOfThreads = 0;
int curr_id = 1;
struct itimerval timer;
struct sigaction sa;
struct node* curr_running_node;
int main_init = 0;
int timer_init = 0;


void new_node(){
  tcb new_thread;
  temp = (struct node*) malloc (1 * sizeof(struct node));
  temp->next = NULL;
}

void printQueue(){
//Print the current list
	printf("Printing current thread list: \n");
  struct node* ptr = head;
  do
  {
	  printf("thread %d \t", ptr->n->thread_id);
		ptr = ptr->next;
  } while (ptr != head);
  printf("\n");
}

struct node* enqueue ( tcb * new_thread){
	printf("Adding thread %d to list: \n", new_thread->thread_id);
  if(head == NULL){
    new_node();
    head = temp;
    tail = temp;
    head->next = head;
    head->prev = head;
  }else{
    new_node();
    tail->next = temp;
	  temp->prev = tail;
	  temp->next = head;
	  head->prev = tail;
    head = temp;
  }
  temp->n = new_thread;

  //printf("the head: %d \n", head->n->thread_id);
  printQueue();

  return temp;
}


struct node* searchNextBlock(){
  printf("Looking for next ready thread\n");
  printf("Curr num of threads: %d\n", numOfThreads);

  struct node* ptr = head;
  do
  {
	  //printf("thread %d \n", ptr->n->thread_id);
    if(ptr->n->thread_status == READY){
      printf("Found the next thread: %d\n", ptr->n->thread_id); 
      return ptr;
    }
		ptr = ptr->next;
  } while (ptr != head);
  return NULL;
}

struct node* requeue(){
  // REQUEUE THE CURR BLOCK BASED ON PRIORITY BEFORE SWAPPING
  printf("Thread at head: %d\n", head->n->thread_id);
  printf("Priority at head: %d\n", head->n->thread_priority);
  printf("Priority of current thread: %d\n", curr_running_node->n->thread_priority);
  printf("Thread at tail: %d\n", tail->n->thread_id);
  printf("Priority at tail: %d\n", tail->n->thread_priority);

  struct node* ptr = head;
  do
  {
    if(ptr->n->thread_priority > curr_running_node->n->thread_priority){
      ptr->prev->next = curr_running_node;
      curr_running_node->next = ptr;
      curr_running_node->prev = ptr->prev;
      ptr->prev = curr_running_node;

      return head;
    }else{
      ptr = ptr->next;
    }
  } while (ptr != head);

  // all threads same priority, insert at end, ptr is at head
  if(curr_running_node == head){
    head = curr_running_node->next;
    tail = curr_running_node;
  }else{
    ptr->prev->next = curr_running_node;
    curr_running_node->next = ptr;
    curr_running_node->prev = ptr->prev;
    ptr->prev = curr_running_node;
    tail = curr_running_node;
  }
  printQueue();

  return head;
}

void ring(int signum, siginfo_t *nfo, void *context){
  timer.it_value.tv_sec = 0;
  setitimer(ITIMER_PROF, &timer, NULL);
	printf("RING RING! The timer has gone off\n");
  printf("The current running node: %d\n", curr_running_node->n->thread_id);
  

  //LETS BACKUP CONTEXT OF RUNNING THREAD
  ucontext_t* old = curr_running_node->n->thread_ctx;
  ucontext_t* updated = (ucontext_t*) context;

  old->uc_stack.ss_sp = updated->uc_stack.ss_sp;

  requeue();

  struct node* next_block = searchNextBlock();
  if(next_block == NULL){
    printf("no next ready\n");
    return;
  }
  printf("Next thread is: %d\n", next_block->n->thread_id);

  //UPDATE THREAD STATUS
  if(curr_running_node->n->thread_status !=  BLOCKED){
  curr_running_node->n->thread_status = READY;
  }
  next_block->n->thread_status = SCHEDULED;

  //LOWER PRIORITY FOR NEXT THREAD
  next_block->n->thread_priority++;

  //UPDATE CURRENT NODE
  struct node* prev_node = curr_running_node;
  curr_running_node = next_block;

  // RESET TIMER
  timer.it_value.tv_sec = 2;
  timer.it_value.tv_usec = 0;
  setitimer(ITIMER_PROF, &timer, NULL);
  printf("Reseted timer before swap\n");
  
  // swapping prev and new current
  swapcontext(prev_node->n->thread_ctx, curr_running_node->n->thread_ctx);


  //sleep(10);
	return;
	//setitimer(ITIMER_PROF, &timer, NULL);
}

tcb* init_tcb(mypthread_t* thread_id){
  // Allocate for tcb
    tcb* new_thread = (tcb*) malloc(sizeof(tcb*));
	  printf("TCB ALLOCATED \n");
    *thread_id = numOfThreads++;
    new_thread->thread_id = *thread_id;

		// Allocate for context
    ucontext_t* ctx = (ucontext_t*) malloc(sizeof(ucontext_t));
    new_thread->thread_ctx = ctx;
		printf("CONTEXT ALLOCATED \n");
    if(getcontext(ctx) == -1){
      printf("Error getting contxt\n");
    }

		// Allocate the stack
    new_thread->thread_ctx->uc_stack.ss_sp = (char*) malloc(sizeof(char) * STACK_SIZE);
		new_thread->thread_ctx->uc_stack.ss_size = STACK_SIZE;
    new_thread->thread_ctx->uc_link = NULL;
		printf("STACK ALLOCATED \n");

    new_thread->thread_status = READY;
    new_thread->thread_priority = 0;
    new_thread->join_thread = -1;
    void **return_ptr  = (void**)malloc(sizeof(void));
    new_thread->return_ptr = NULL;

    return new_thread;
}

int init_timer(int sec, int ms){
  // REGISTERING SIGNAL HANDLER
  memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &ring;
	sigaction (SIGPROF, &sa, NULL);
  printf("SIGNAL HANDLER INITIALIZED\n");

  timer.it_interval.tv_usec = ms; 
	timer.it_interval.tv_sec = sec;

  timer.it_value.tv_usec = ms;
	timer.it_value.tv_sec = sec;
  
  //setitimer(ITIMER_PROF, &timer, NULL);
  printf("TIMER INITIALIZED\n");
  return 1;
}

tcb* init_main_thread(){
  mypthread_t* main_thread = (mypthread_t*) malloc(sizeof(mypthread_t));
  tcb* main_tcb = init_tcb(main_thread);

  main_tcb->thread_status = SCHEDULED;
  main_tcb->thread_priority++;
  curr_running_node = enqueue(main_tcb);
  
  return main_tcb;
}

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr,
                      void *(*function)(void*), void * arg) {
    printf("IN MYPTHREAD_CREATE:\n");

    if(timer_init == 0){
      //INITIALIZE TIMER
      init_timer(2,0); //2 secs and 0 ms
      timer_init = 1;
    }

    if(main_init == 0){
      //FIRST CALL, INITIALIZE MAIN CONTEXT
      printf("FIRST CALL, INITIALIZE MAIN CONTEXT\n");
      tcb* main_tcb = init_main_thread();


      //INITIALIZE ACTUAL THREAD BEING CREATED
      tcb* thread_tcb = init_tcb(thread);
      makecontext(thread_tcb->thread_ctx,(void (*)()) function, 1, arg);
      enqueue(thread_tcb);

      //GET MAIN CONTEXT AND RETURN
      main_init = 1;
      // SET TIMER
      setitimer(ITIMER_PROF, &timer, NULL); 
      getcontext(main_tcb->thread_ctx);
      return 1;   
    }

    // ALLOCATE AND INIT TCB FOR THREAD
    tcb* thread_tcb = init_tcb(thread);
    makecontext(thread_tcb->thread_ctx,(void (*)()) function, 1, arg);

		// Add to list..
	 	enqueue(thread_tcb);
    return 1;
}


/* give CPU possession to other user-level threads voluntarily */

int mypthread_yield() {

  printf("IN YIELD:\n");
  raise(SIGPROF);
	// YOUR CODE HERE
	return 0;
};
/*
void dequeue(int threadID){
  printf("Entering dequeue\n");
  //CHECK IF THE NODE IS THE ONLY NODE
  temp = head;
  if(temp->next == head{
      head = NULL;
      return;
    }
    //CHECK IF IT IS THEFIRST NODE
    struct node *ptr1;
    if(temp == head){
      ptr1 = head;
      while(ptr1->next != head)
	ptr1 = ptr1->next;
	head  = temp->next;
	tail->next = head;
	head->prev = tail;
      
      //CHECK IF IT IS THE LAST NODE
	if(temp->next =){
	}*/

    tcb *search(int threadID){
      printf("ENTERING SEARCH FUNCTION\n");
      temp = head;

      while(temp != NULL){ 
	if(temp->n->thread_id == threadID){
	  printf("EXITING SEARCH\n");
	  return temp->n;
	   printf("search thread is is:%d\n", temp->n->thread_id);
	}else{
	  temp = temp->next;

	}
      }
      printf("EXITING SEARCH FUNCTION\n");
      return NULL;
    }




    /* terminate a thread */
    void mypthread_exit(void *value_ptr) {

      // Deallocated any dynamic memory created when starting this thread
      printf("ENTERING THE EXIT FUNCTION\n");
      int thread1_id = curr_running_node->n->join_thread; 
      printf("JOINID on thread that was called join on: %d\n", thread1_id);
      tcb * thread1;
      thread1 = search(thread1_id); //THIS IS THE THREAD THAT CALLED JOIN ON CURRENT RUNNING THREAD
      tcb * deletethread;
      tcb * next_running_thread;
      if(thread1_id != -1){ //THIS IS THE THREAD THAT WAS CALLED JOINED ON
       thread1->return_ptr =(void**)&value_ptr;
       thread1->thread_status = READY;

      }
      deletethread = curr_running_node;
      //next_running_thread = searchNextBlock();
      curr_running_node = thread1;
      raise(SIGPROF);
      free(deletethread);
 
  printf("LEAVING EXIT FUNCTION\n");
	// YOUR CODE HERE
};

 
/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {
  printf("Entering the join function\n");
  
  tcb * target;
  printf("check1\n");
  target = search(thread);
  printf("Thread on which join was called id: %d\n", target->thread_id);

  if(target == NULL){ //TARGET THREAD IS NOT IN THE LIST
    return 1;
  }else{
    curr_running_node->n->thread_status = BLOCKED;
    target->join_thread = curr_running_node->n->thread_id; //THIS THREAD WAS CALLED JOINED ON AND NEED A REFRENCE IN EXIT.
    printf("Target joinid updated: %d\n", target->join_thread);
    target->return_ptr = value_ptr;
    raise(SIGPROF);
  }
  printf("Exiting the Join\n");

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
