/*
 * Thread test code.
 */
#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>

static struct semaphore *tsem = NULL;
static struct spinlock tspinlock;
static int counter;

static
void
init_sem(void)
{
	if (tsem==NULL) {
		tsem = sem_create("tsem", 0);
		if (tsem == NULL) {
			panic("stc: sem_create failed\n");
		}
	}
        spinlock_init(&tspinlock);

}

static
void
countingthread(void *junk, unsigned long times)
{
	(void)junk;

	for (int i = 0; i < (long)times; i++) {
		spinlock_acquire(&tspinlock);
		counter++;
		spinlock_release(&tspinlock);
	}
	V(tsem);
}

static
void
runthreads(int threads, unsigned long times)
{
	char name[16];
	int i, result;

	for (i=0; i<threads; i++) {
		snprintf(name, sizeof(name), "stc%d", i);
		result = thread_fork(name, NULL, countingthread, NULL, times);
		if (result) {
			panic("stc: thread_fork failed %s)\n", 
			      strerror(result));
		}
	}

	for (i=0; i<threads; i++) {
		P(tsem);
	}
}


int
spinthreadcounter(int nargs, char **args)
{
        if (nargs < 2 || nargs > 3) {
                kprintf("Usage: stc [number of threads]\n");
                return 0;
        }

	int torun = atoi(args[1]);
	unsigned long runtimes = 1;

	if (nargs == 3) {
		runtimes = atoi(args[2]);
	}

	(void)nargs;
	(void)args;

	counter = 0;
	init_sem();
	kprintf("Starting spinlock thread counter test...\n");
	runthreads(torun, runtimes);
	kprintf("%d", counter);
	kprintf("\nTest complete, mutual exclusion expected\n");

	return 0;
}
