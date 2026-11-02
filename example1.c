#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Example 1: Usage of pthread_create and pthread_join
// Creates N threads, each thread generates a random number x,
// and sleeps for x seconds before joining the main thread.

typedef struct thread_arg {
    int tid;
    int val;
} arg_t;

void *run(void *arg)
{
    arg_t *a = (arg_t *) arg;

    // rand() isn't guaranteed to be thread-safe, but it is on Linux with glibc.
    int x = rand() % 10 + 1;

    sleep(x);
    a->val = x;

    printf("Hello from thread %d: My value is %d.\n", a->tid, a->val);

    return NULL;
}

int main()
{
    int num_threads = 4;

    pthread_t *threads = calloc(num_threads, sizeof(pthread_t));
    arg_t *args = calloc(num_threads, sizeof(arg_t));

    if (!threads || !args) {
        fprintf(stderr, "Malloc failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_threads; ++i) {
        args[i].tid = i;

        int error = pthread_create(
            &threads[i], // pthread_create will write the thread identifier here.
            NULL,   // can pass in pthread_attr_t object for advanced use.
            run,    // routine that the thread runs.
            &args[i]    // pass in arg to the routine.
        );

        // pthread_create returns 0 if successful.
        if (error) {
            fprintf(stderr, "pthread_create: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_threads; ++i) {
        // pthread_join is blocking, so even if thread 1 finishes before thread 0,
        // we will be waiting for pthread_join(threads[0]) to return.
        int error = pthread_join(threads[i], NULL);

        if (error) {
            fprintf(stderr, "pthread_join: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    int total = 0;

    for (int i = 0; i < num_threads; ++i) {
        total += args[i].val;
    }

    printf("Total is: %d\n", total);

    free(threads);
    free(args);

    return 0;
}