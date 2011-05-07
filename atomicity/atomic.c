#include <pthread.h>
#include <stdio.h>

pthread_mutex_t sharedVariableMutex;
int gSharedVariable = 0;

void incrementTask(void *);
void decrementTask(void *);

#define USE_MUTEX 0

void incrementTask(void *param)
{
	while (1) {
	/* Delay for 3 seconds. */
	sleep(3);

	/* Wait for the mutex before accessing the GPIO registers. */
#if USE_MUTEX
	pthread_mutex_lock(&sharedVariableMutex);
#endif

	gSharedVariable++;

	printf("Increment Task: shared variable value is %d\n", gSharedVariable);

	/* Release the mutex for other task to use. */
#if USE_MUTEX
	pthread_mutex_unlock(&sharedVariableMutex);
#endif
	}
}

void decrementTask(void *param)
{
	while (1) {
		sleep(2);

		/* Wait for the mutex to become available. */
#if USE_MUTEX
		pthread_mutex_lock(&sharedVariableMutex);
#endif

		gSharedVariable--;

		printf("Decrement Task: shared variable value is %d\n", gSharedVariable);

		/* Release the mutex. */
#if USE_MUTEX
		pthread_mutex_unlock(&sharedVariableMutex);
#endif
	}
}

int main(void)
{
	pthread_t incrementTaskObj, decrementTaskObj;

	pthread_mutex_init(&sharedVariableMutex, NULL);

	pthread_create(&incrementTaskObj, NULL, (void *)incrementTask, NULL);
	pthread_create(&decrementTaskObj, NULL, (void *)decrementTask, NULL);

	pthread_join(incrementTaskObj, NULL);
	pthread_join(decrementTaskObj, NULL);

	return 0;
}
