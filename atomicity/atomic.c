#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

pthread_mutex_t sharedVariableMutex;
int gSharedVariable = 0;

void incrementTask(void *);
void decrementTask(void *);

void incrementTask(void *param)
{
	while (1) {
		sleep(1);

#if USE_MUTEX
		pthread_mutex_lock(&sharedVariableMutex);

		++gSharedVariable;
		
		pthread_mutex_unlock(&sharedVariableMutex);
#elif USE_ASM
		asm volatile(
			"lock addl %1, %0\n"
			: "=m" (gSharedVariable)
			: "ir" (1), "m" (gSharedVariable)
		);
#else
		++gSharedVariable;
#endif
	printf("Increment Task: shared variable value is %d\n", gSharedVariable);
	}
}

void decrementTask(void *param)
{
	while (1) {
		sleep(2);

#if USE_MUTEX
		pthread_mutex_lock(&sharedVariableMutex);

		--gSharedVariable;

		pthread_mutex_unlock(&sharedVariableMutex);
#elif USE_ASM
		asm volatile(
			"lock subl %1, %0\n"
			: "=m" (gSharedVariable)
			: "ir" (1), "m" (gSharedVariable)
		);
#else
		--gSharedVariable;
#endif
		printf("Decrement Task: shared variable value is %d\n", gSharedVariable);
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
