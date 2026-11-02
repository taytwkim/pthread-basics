#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Example 2: Usage of pthread_detach
// Creates N threads, each thread generates a random number x
// and sleeps for x seconds.

typedef struct thread_arg {
    int tid;
} arg_t;

void *run(void *arg)
{
    arg_t *a = (arg_t *) arg;

    // rand() isn't guaranteed to be thread-safe, but it is on Linux with glibc.
    int x = rand() % 10 + 1;

    sleep(x);

    printf("Hello from thread %d: My value is %d.\n", a->tid, x);

    free(a);

    return NULL;
}

int main()
{
    int num_threads = 4;

    for (int i = 0; i < num_threads; ++i) {
        // We only use this thread ID within this iteration's scope,
        // so a local variable is enough.
        pthread_t thread;

        // Since threads will be detached,
        // we have to free memory inside the routine.
        arg_t *arg = calloc(1, sizeof(arg_t));

        if (!arg) {
            fprintf(stderr, "Malloc failed.\n");
            exit(EXIT_FAILURE);
        }

        arg->tid = i;

        int error = pthread_create(
            &thread, // pthread_create will write the thread identifier here.
            NULL,   // can pass in pthread_attr_t object for advanced use.
            run,    // routine that the thread runs.
            arg    // pass in arg to the routine.
        );

        // pthread_create returns 0 if successful.
        if (error) {
            fprintf(stderr, "pthread_create: %s\n", strerror(error));
            free(arg);
            exit(EXIT_FAILURE);
        }

        // pthread_detach returns 0 if successful.
        error = pthread_detach(thread);

        if (error) {
            fprintf(stderr, "pthread_detach: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    // Returning from main would terminate the entire process,
    // including any workers that are still running.
    // pthread_exit terminates only the main thread.
    // The process exits when the last thread finishes.
    pthread_exit(NULL);
}