#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Example 5: Usage of pthread_barrier
// All threads need to finish the first phase (prints "Ready")
// before moving on to the second phase (prints "Starting next phase").

pthread_barrier_t barrier;

typedef struct thread_arg {
    int tid;
} arg_t;

void *run(void *arg)
{
    arg_t *a = (arg_t *) arg;

    int x = rand() % 10 + 1;
    sleep(x);

    printf("Thread %d: Ready\n", a->tid);

    pthread_barrier_wait(&barrier);

    printf("Thread %d: Starting next phase\n", a->tid);

    return NULL;
}

int main()
{
    int num_threads = 4;
    int error = pthread_barrier_init(&barrier, NULL, num_threads);

    if (error) {
        fprintf(stderr, "pthread_barrier_init: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    pthread_t *threads = calloc(num_threads, sizeof(pthread_t));
    arg_t *args = calloc(num_threads, sizeof(arg_t));

    if (!threads || !args) {
        fprintf(stderr, "Malloc failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_threads; ++i) {
        args[i].tid = i;

        error = pthread_create(
            &threads[i],
            NULL,
            run,
            &args[i]
        );

        if (error) {
            fprintf(stderr, "pthread_create: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_threads; ++i) {
        error = pthread_join(threads[i], NULL);

        if (error) {
            fprintf(stderr, "pthread_join: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }
    
    pthread_barrier_destroy(&barrier);
    free(threads);
    free(args);

    return 0;
}