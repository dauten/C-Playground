/* On Mac OS (aka OS X) the ucontext.h functions are deprecated and requires the
   following define.

   On Mac OS when compiling with gcc (clang) the -Wno-deprecated-declarations
   flag must also be used to suppress compiler warnings.
*/
#define _XOPEN_SOURCE 700

#include <signal.h>   /* SIGSTKSZ (default stack size), MINDIGSTKSZ (minimal
                         stack size) */
#include <stdio.h>    /* puts(), printf(), fprintf(), perror(), setvbuf(), _IOLBF,
                         stdout, stderr */
#include <stdlib.h>   /* exit(), EXIT_SUCCESS, EXIT_FAILURE, malloc(), free() */
#include <ucontext.h> /* ucontext_t, getcontext(), makecontext(),
                         setcontext(), swapcontext() */

/* Stack size for each context. */
#define STACK_SIZE SIGSTKSZ

/* Number of iterations for functions foo() and bar(). */
#define N 5

/* Two contexts. */
ucontext_t foo_ctx, yee_ctx, bar_ctx, scheduler;
ucontext_t *threads[3];
int thread_num;


int yieldcontext(ucontext_t active){
  printf("yeilded to scheduler\n");
  return swapcontext(&active, &scheduler);
}
/* Function executed by the foo context. */
void foo() {
  for (int i = 0; i < N; i++) {
    printf("foo (%d)\n", i);

    /* Hand over control to the bar context */
    if (swapcontext(&foo_ctx, &scheduler) < 0) {
      perror("swapcontext");
      exit(EXIT_FAILURE);
    }

  }
}

/* Function executed by the foo context. */
void yee() {
  for (int i = 0; i < N; i++) {
    printf("yee (%d)\n", i);

    /* Hand over control to the bar context */
    if (swapcontext(&yee_ctx, &scheduler) < 0) {
      perror("swapcontext");
      exit(EXIT_FAILURE);
    }

  }
}

/* Function used by the bar context. */
void bar() {
  for (int i = 0; i < N; i++) {
    printf("bar (%d)\n", i);

    /* Hand over control to the foo context */
    if (swapcontext(&bar_ctx, &scheduler) < 0) {
      perror("swapcontext");
      exit(EXIT_FAILURE);
    }

  }
}

/* Function used by the foo_done context. */
void foo_done(char *msg) {
  printf("foo  -  %s!\n", msg);
}

/* Initialize a context.

   ctxt - context to initialize.

   next - successor context to activate when ctx returns. If NULL, the thread
          exits when ctx returns.
 */
void init_context(ucontext_t *ctx, ucontext_t *next) {
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

  ctx->uc_link           = next;
  ctx->uc_stack.ss_sp    = stack;
  ctx->uc_stack.ss_size  = STACK_SIZE;
  ctx->uc_stack.ss_flags = 0;
}

/* Initialize context ctx  with a call to function func with zero argument.
 */
void init_context0(ucontext_t *ctx, void (*func)(), ucontext_t *next) {
  init_context(ctx, next);
  makecontext(ctx, func, 0);
}

/* Initialize context ctx with a call to function func with one string argument.
 */
void init_context1(ucontext_t *ctx, void (*func)(), const char *str, ucontext_t *next) {
  init_context(ctx, next);
  makecontext(ctx, func, 1, str);
}

/* Link context a to context b such that when context a returns context b is resumed.
 */
void link_context(ucontext_t *a, ucontext_t *b) {
  a->uc_link = b;
}

void round_robin(){
  while(1){
    for(int i = 0 ; i < thread_num; i++){
      swapcontext(&scheduler, threads[i]);

      //swapcontext(&scheduler, &bar_ctx);
      //swapcontext(&scheduler, &foo_ctx);
    }
  }
}

int main() {
  ucontext_t foo_done_ctx;

  /* Flush each printf() as it happens. */
  setvbuf(stdout, 0, _IOLBF, 0);

  thread_num = 3;
  /* Initialize contexts(). */
  init_context1(&foo_done_ctx, foo_done, "done", NULL);
  init_context0(&foo_ctx, foo, &foo_done_ctx);
  init_context0(&bar_ctx, bar, NULL);
  init_context0(&yee_ctx, yee, NULL);
  init_context0(&scheduler, round_robin, NULL);

  threads[0] = &foo_ctx;
  threads[1] = &bar_ctx;
  threads[2] = &yee_ctx;

  /* Transfers control to the foo context. */
  setcontext(&scheduler);

  fprintf(stderr, "ERROR! A successful call to setcontext() does not return!\n");
}
