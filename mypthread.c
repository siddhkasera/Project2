#include "mypthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdatomic.h>


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
	  head->prev = temp;
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

struct node* requeue(struct node* curr_node){
  // REQUEUE THE CURR BLOCK BASED ON PRIORITY BEFORE SWAPPING
  printf("Thread at head: %d\n", head->n->thread_id);
  printf("Priority at head: %d\n", head->n->thread_priority);
  printf("Priority of current thread: %d\n", curr_node->n->thread_priority);
  printf("Thread at tail: %d\n", tail->n->thread_id);
  printf("Priority at tail: %d\n", tail->n->thread_priority);
  printQueue();

  struct node* ptr = head;
  do
  {
    if(ptr->n->thread_priority > curr_node->n->thread_priority){
      ptr->prev->next = curr_node;
      curr_node->next = ptr;
      curr_node->prev = ptr->prev;
      ptr->prev = curr_node;

      return head;
    }else{
      ptr = ptr->next;
    }
  } while (ptr != head);

  // all threads same priority, insert at end, ptr is at head
  printf("came here\n");
  if(curr_node == head){
    head = curr_node->next;
    tail = curr_node;
    head->prev = tail;
    tail->next = head;
  }else{
    
    if(ptr->next == curr_node){
      ptr->next = curr_node->next;
    }
    curr_node->prev = ptr->prev;
    ptr->prev->next = curr_node;
    curr_node->next = ptr;
    
    ptr->prev = curr_node;
    tail = curr_node;
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

  requeue(curr_running_node);

  struct node* next_block = searchNextBlock();
  if(next_block == NULL){
    printf("no next ready\n");
    return; //TODO: RETURN OR EXIT
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
    printf("INITIALIZING TCB\n");
    // Allocate for tcb
    tcb* new_thread = (tcb*) malloc(sizeof(tcb));

    //tcb* new_thread = (tcb*) malloc(sizeof(tcb*));
	  printf("TCB ALLOCATED \n");
    *thread_id = numOfThreads++;
    new_thread->thread_id = *thread_id;

		// Allocate for context
    new_thread->thread_ctx = (ucontext_t*) malloc(sizeof(ucontext_t));

    // Allocate the stack
    new_thread->thread_ctx->uc_stack.ss_sp = (char*) malloc(sizeof(char) * STACK_SIZE);
    new_thread->thread_ctx->uc_stack.ss_size = STACK_SIZE;
    new_thread->thread_ctx->uc_link = NULL;
    printf("STACK ALLOCATED \n");
		printf("CONTEXT ALLOCATED \n");
    if(getcontext(new_thread->thread_ctx) == -1){
      printf("Error getting contxt\n");
    }

    new_thread->thread_status = READY;
    new_thread->thread_priority = 0;
    new_thread->join_thread = -1;
    new_thread->return_ptr = (void**)malloc(sizeof(void**));

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

    tcb* main_tcb;
    if(main_init == 0){
      //FIRST CALL, INITIALIZE MAIN CONTEXT
      printf("FIRST CALL, INITIALIZE MAIN CONTEXT\n");
      main_tcb = init_main_thread();


      //INITIALIZE ACTUAL THREAD BEING CREATED
      // tcb* thread_tcb = init_tcb(thread);
      // makecontext(thread_tcb->thread_ctx,(void (*)()) function, 1, arg);
      // enqueue(thread_tcb);

      //free((ucontext_t*)thread_tcb->thread_ctx);
      //GET MAIN CONTEXT AND RETURN
      // main_init = 1;
      // // SET TIMER
      // setitimer(ITIMER_PROF, &timer, NULL); 
      // getcontext(main_tcb->thread_ctx);
      // return 1;   
    }

    // ALLOCATE AND INIT TCB FOR THREAD
    tcb* thread_tcb = init_tcb(thread);
    makecontext(thread_tcb->thread_ctx,(void (*)()) function, 1, arg);

		// Add to list..
	 	enqueue(thread_tcb);

    if(main_init == 0){
      main_init = 1;
      // SET TIMER
      setitimer(ITIMER_PROF, &timer, NULL); 
      getcontext(main_tcb->thread_ctx);
    }
    return 1;
}


/* give CPU possession to other user-level threads voluntarily */

int mypthread_yield() {

  printf("IN YIELD:\n");
  raise(SIGPROF);
	// YOUR CODE HERE
	return 0;
};

void dequeue(int threadID){
  printQueue();

  struct node* ptr = head;
  do
  {
    if(ptr->n->thread_id == threadID){
      printf("Found thread to deq\n");
      ptr->prev->next = ptr->next;
      ptr->next->prev = ptr->prev;

      if(ptr == head){
        printf("Ptr was the head\n");
        head = ptr->next;
      }
      if(ptr == tail){
        printf("Ptr was the tail\n");
        tail = ptr->prev;
      }
      printQueue();

      printf("Ptr i want to free: %d\n", ptr->n->thread_id);
      free(ptr->n->thread_ctx->uc_stack.ss_sp);
      printf("freed stack\n");
      free(ptr->n->thread_ctx);
      printf("freed ctx\n");
      free(ptr->n->return_ptr);
      free(ptr->n);
      free(ptr);
      return;

    }else{
      ptr = ptr->next;
    }
  } while (ptr != head);

  
  return;
	}

    struct node *search(int threadID){
      printf("ENTERING SEARCH FUNCTION\n");
      temp = head;

      while(temp != NULL){ 
	      if(temp->n->thread_id == threadID){
	        printf("EXITING SEARCH\n");
	        return temp;
	      }else{
	        temp = temp->next;

	      }
      }
      printf("EXITING SEARCH FUNCTION\n");
      return NULL;
    }




    /* terminate a thread */
    void mypthread_exit(void *value_ptr) {

      printf("ENTERING THE EXIT FUNCTION\n");
      int callingThreadId = curr_running_node->n->join_thread; 
      printf("JOINID of thread that called join: %d\n", callingThreadId);
      if(callingThreadId == -1){
        curr_running_node->n->thread_status = DEAD;
        if(value_ptr != NULL){
           *(curr_running_node->n->return_ptr) = value_ptr;
        }
      }
      else{
        struct node * callingThread;
        callingThread = search(callingThreadId); //THIS IS THE THREAD THAT CALLED JOIN ON CURRENT RUNNING THREAD
        if(value_ptr != NULL){
          printf("in exit valueptr is: %d\n", *(int*)value_ptr);
          *(curr_running_node->n->return_ptr) = value_ptr;
          printf("in exit curr threads retptr is: %d\n", *(int*)(*(curr_running_node->n->return_ptr)));
        }
        callingThread->n->thread_status = READY;
      
        printf("Calling thread's status updated to: %d\n", callingThread->n->thread_status);
        printQueue();
      }
      
      //Find next ready block
      struct node * next_block = searchNextBlock();
      
      if(next_block == NULL){
        return;
      }
      printf("Next thread is: %d\n", next_block->n->thread_id);
      next_block->n->thread_status = SCHEDULED;

      //LOWER PRIORITY FOR NEXT THREAD
      next_block->n->thread_priority++;
      curr_running_node = next_block;

      setcontext(curr_running_node->n->thread_ctx);
 
  printf("LEAVING EXIT FUNCTION\n");
	// YOUR CODE HERE
};

 
/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {
  printf("Entering the join function\n");
  
  struct node * target;
  target = search(thread);
  printf("Thread on which join was called id: %d\n", target->n->thread_id);

  if(target == NULL){ //TARGET THREAD IS NOT IN THE LIST(should not happen)
    return 1;
  }
  else if(target->n->thread_status == DEAD){ //THREAD ALREADY RAN AND EXITED
    value_ptr = target->n->return_ptr;
    dequeue(thread);
    return 0;

  }
  else{
    curr_running_node->n->thread_status = BLOCKED;
    target->n->join_thread = curr_running_node->n->thread_id; //THIS THREAD WAS CALLED JOINED ON AND NEED A REFRENCE IN EXIT.
    printf("Target joinid updated: %d\n", target->n->join_thread);
    *(target->n->return_ptr) = value_ptr;
    raise(SIGPROF);
  }
  
  printf("from join, value_ptr holds: %d\n", *(value_ptr));

  dequeue(thread);
  printf("Exiting the Join\n");
  return 0;
};

/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex,
                          const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex

  printf("MUTEX INITIALIZED\n");
  if(mutex == NULL){
    printf("Mutext not malloced\n");
    return 1;
  }  

  //initialize
  mutex->init = 1;
  //mutex->waitHead = (struct node*) malloc(sizeof(node));
  mutex->locked = 0; //NOT LOCKED
  mutex->currMutThread = NULL;

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
        printf("STARTED LOCK\n");
        if(mutex->init != 1){
          printf("Mutex not initialized!\n");
          return 0;
        }

        //if(mutex->locked == 1){
	while(__atomic_test_and_set(&mutex->locked, 0)){
	printf("mutex already locked, adding to wait\n");
          curr_running_node->n->thread_status = BLOCKED; 
          struct node* threadToWait = (struct node*) malloc(sizeof(struct node));
          threadToWait->n = curr_running_node->n;
          struct node* ptr = mutex->waitHead;
          if(ptr != NULL){
            threadToWait->next = ptr;
            mutex->waitHead = threadToWait;
          }else{
            mutex->waitHead = threadToWait;
          }
          raise(SIGPROF);
        }

        mutex->currMutThread = curr_running_node;
        //mutex->locked = 1; //TODO: TEST & SET??
        printf("MUTEX LOCKED BY THREAD %d\n", curr_running_node->n->thread_id);

        return 1;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.

	// YOUR CODE HERE
  if(mutex->init != 1){
    printf("Mutex not initialized!\n");
    return 0;
  }

  if(mutex->locked == 0){
    printf("Mutex already unlocked\n");
    return 0;
  }

  //READYS THE WAITLIST
  struct node* ptr = mutex->waitHead;
  while(ptr != NULL){
    ptr->n->thread_status = READY;
    ptr = ptr->next;
  }
  printf("MUTEX UNLOCKED BY THREAD %d\n", curr_running_node->n->thread_id);
  //FREE LIST
  ptr = mutex->waitHead;
  while (ptr != NULL)
  {
    struct node* curr = ptr;
    ptr = ptr->next;
    free(curr);
  }
  

  mutex->currMutThread = NULL;
  mutex->locked = 0; //TODO: TEST AND SET??

	return 1;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in mypthread_mutex_init

  mutex->init = 0;
  // waitlist already freed when unlocked

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
