/* On Mac OS (aka OS X) the ucontext.h functions are deprecated and requires the
   following define.
*/
#define _XOPEN_SOURCE 700

/* On Mac OS when compiling with gcc (clang) the -Wno-deprecated-declarations
   flag must also be used to suppress compiler warnings.
*/

#include <signal.h>   /* SIGSTKSZ (default stack size), MINDIGSTKSZ (minimal
                         stack size) */
#include <stdio.h>    /* puts(), printf(), fprintf(), perror(), setvbuf(), _IOLBF,
                         stdout, stderr */
#include <stdlib.h>   /* exit(), EXIT_SUCCESS, EXIT_FAILURE, malloc(), free() */
#include <ucontext.h> /* ucontext_t, getcontext(), makecontext(),
                         setcontext(), swapcontext() */
#include <stdbool.h>  /* true, false */

#include "sthreads.h"

/* Stack size for each context. */
#define STACK_SIZE SIGSTKSZ*100
#define THREAD_LIMIT 10

/*******************************************************************************
                             Global data structures

                Add data structures to manage the threads here.
********************************************************************************/
//Three sample contexts, plus schedular, current global, and list of threads
ucontext_t scheduler;
ucontext_t *current;
static ucontext_t threads[THREAD_LIMIT];
static int num_of_threads = 0;




/*******************************************************************************
                             Auxiliary functions

                      Add internal helper functions here.
********************************************************************************/
/*
* Scheduler function, when a thread yields or is preempted
* control is brough back to this function to determine and start next thread
*/
void round_robin(){
  while(1){
    for(int i = 0 ; i < num_of_threads; i++){
      current = &threads[i];
      swapcontext(&scheduler, &threads[i]);
    }
  }
}

/* Initialize a context.

   ctxt - context to initialize.

   next - successor context to activate when ctx returns. If NULL, the thread
          exits when ctx returns.
 */
void init_context(ucontext_t *ctx) {
  /* Allocate memory to be used as the stack for the context. */
  void *stack = malloc(STACK_SIZE);

  if (stack == NULL) {
    perror("Allocating stack");
    exit(EXIT_FAILURE);
  }

  if (getcontext(ctx) < 0) {
    perror("getcontext");
    exit(EXIT_FAILURE);
  }

  /* Before invoking makecontext(ctx), the caller must allocate a new stack for
     this context and assign its address to ctx->uc_stack, and define a successor
     context and assigns address to ctx->uc_link.
  */

  ctx->uc_link           = NULL;
  ctx->uc_stack.ss_sp    = stack;
  ctx->uc_stack.ss_size  = STACK_SIZE;
  ctx->uc_stack.ss_flags = 0;
}

/* Initialize context ctx  with a call to function func with zero argument.
 */
void init_context0(ucontext_t *ctx, void (*func)(), ucontext_t *next) {
  init_context(ctx);
  makecontext(ctx, func, 0);
}

/* Initialize context ctx with a call to function func with one string argument.
 */
void init_context1(ucontext_t *ctx, void (*func)(), const char *str, ucontext_t *next) {
  init_context(ctx);
  makecontext(ctx, func, 1, str);
}


/*******************************************************************************
                    Implementation of the Simple Threads API
********************************************************************************/


int init(){
  setvbuf(stdout, 0, _IOLBF, 0);
  init_context0(&scheduler, round_robin, NULL);
  setcontext(&scheduler);
  return -1; //error, should not return
}


tid_t spawn(void (*start)()){
  init_context0(&threads[num_of_threads], start, NULL);
  num_of_threads++;
  return 1; //true
}

void yield(){
  swapcontext(current, &scheduler);
}

void  done(){
  while(true){
    yield(); //messy and poor form but it works for now, OK?
  }
}

tid_t join() {
  return -1;
}
