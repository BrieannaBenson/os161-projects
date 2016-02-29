/*
 * Thread test code.
 */
#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>

static struct semaphore *tsem = NULL;

static
void
init_sem(void)
{
	if (tsem==NULL) {
		tsem = sem_create("tsem", 0);
		if (tsem == NULL) {
			panic("dontpanic: sem_create failed\n");
		}
	}
}

static
void
countingthread(void *junk, unsigned long num)
{

	int ch = '0' + num;
	(void)junk;
	putch(ch);

	V(tsem);
}

static
void
runthreads(int threads)
{
	char name[16];
	int i, result;

	for (i=0; i<threads; i++) {
		snprintf(name, sizeof(name), "dontpanic%d", i);
		result = thread_fork(name, NULL, countingthread, NULL, i);
		if (result) {
			panic("dontpanic: thread_fork failed %s)\n", 
			      strerror(result));
		}
	}

	for (i=0; i<threads; i++) {
		P(tsem);
	}
}


int
threadtestmulti(int nargs, char **args)
{
        if (nargs < 2 || nargs > 2) {
                kprintf("Usage: dontpanic [number of threads]\n");
                return 0;
        }

	int torun = atoi(args[1]);

	(void)nargs;
	(void)args;

	init_sem();
	kprintf("Starting thread test, don't panic...\n");
	runthreads(torun);
	kprintf("\nThread test done.  Thank you for bringing your towel.\n");

	return 0;
}
