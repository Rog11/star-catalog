#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_function(void *arg);

int main() {
    int num_threads;

    printf("Enter the number of threads to create: ");
    scanf("%d", &num_threads);

    pthread_t threads[num_threads];

    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, thread_function, (void *)&i) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    printf("All threads have completed.\n");

    return 0;
}

void *thread_function(void *arg) {
    int thread_num = *(int *)arg;

    printf("Thread %d has started.\n", thread_num);

    /* Insert thread work here */

    printf("Thread %d has completed.\n", thread_num);

    pthread_exit(NULL);
}
