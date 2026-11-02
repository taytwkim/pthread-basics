#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Example 3: Usage of pthread_mutex
// Creates N threads, each thread generates a random number x,
// and adds to a lock-protected sum.

int total = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct thread_arg {
    int tid;
} arg_t;

void *run(void *arg)
{
    arg_t *a = (arg_t *) arg;

    // rand() isn't guaranteed to be thread-safe, but it is on Linux with glibc.
    int x = rand() % 10 + 1;
    sleep(x);

    pthread_mutex_lock(&mutex);
    // Critical section

    total += x;
    
    printf("Hello from thread %d: Adding %d to total. Total is now %d.\n",
        a->tid, x, total);

    pthread_mutex_unlock(&mutex);

    free(a);

    return NULL;
}

int main()
{
    int num_threads = 4;

    for (int i = 0; i < num_threads; ++i) {
        pthread_t thread;
        arg_t *arg = calloc(1, sizeof(arg_t));

        if (!arg) {
            fprintf(stderr, "Malloc failed.\n");
            exit(EXIT_FAILURE);
        }

        arg->tid = i;

        int error = pthread_create(
            &thread,
            NULL,
            run,
            arg
        );

        if (error) {
            fprintf(stderr, "pthread_create: %s\n", strerror(error));
            free(arg);
            exit(EXIT_FAILURE);
        }

        error = pthread_detach(thread);

        if (error) {
            fprintf(stderr, "pthread_detach: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    pthread_exit(NULL);
}