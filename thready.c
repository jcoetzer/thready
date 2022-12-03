#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <pthread.h>

#define MAX_THREADS 8
#define MESSAGE_SIZE 1024

void usage(char *pname);
void *thread_check( void *ptr );
int run_thread_check();

int main(int argc, char *argv[])
{
	int rc, pnum, tcount;
	if (argc < 3)
	{
		usage(argv[0]);
	}
	pnum = atoi(argv[1]);
	tcount = atoi(argv[2]);
	if (pnum == 0)
	{
		rc = run_thread_check(tcount);
	}
	else
	{
	}
	return rc;
}

void usage(char *pname)
{
	printf("Usage:\n\t%s <PORT> <COUNT>\n", basename(pname));
	exit(1);
}

void *thread_check( void *ptr )
{
	char *message;
	message = (char *) ptr;
	printf("%s \n", message);
}

int run_thread_check()
{
	pthread_t thrd1, thrd2;
	char message1 = "thread 1";
	char *message2 = "thread 2";
	int thready1, thready2;

	thready1 = pthread_create( &thrd1, NULL, thread_check, (void*) message1);
	thready2 = pthread_create( &thrd2, NULL, thread_check, (void*) message2);

	pthread_join( thrd1, NULL);
	pthread_join( thrd2, NULL);

	printf("thread 1 returns: %d\n", thready1);
	printf("thread 2 returns: %d\n", thready2);
	return 0;
}
