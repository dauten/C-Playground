Dale Auten; dauten@siue.edu; https://github.com/dauten/C-Playground/tree/master/CS514/hw2/simplethreads

Compile with `make` and run sthreads_test.o

The threads spawned in sthreads_test main() can be changed to test yeild, completion, and preemption

Usage:
init() starts the round robin scheduler and does not return.  At least one initial thread should be spawned before this.
The total number of threads that can ever have existed defaults to 10.  This can be changed by modifying THREAD_LIMIT in sthreads.c

Summary:
The spawn() functions takes only a function and the program takes care of adding it to the scheduler.
When the timer runs out context is given to the preempt function.  Here we check if the thread who this timer was for already yielded.  If they did, we don't want to interrupt the current thread so we return.  If this thread needs to be preempted, we swap to the next thread.
When a thread yields willingly, context is returned to the round_robin() function.  Here the next thread is calculated, its noted that the current thread yielded so its timer shouldn't interupt the next thread, and a new timer is set.
When a thread has completed execution and calls done(), it maintains its location in the scheduler and merely yields indefinitely.

Much of the code was moving stuff around from timer.c and contexts.c