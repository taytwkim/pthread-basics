#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Example 4: Usage of pthread_cond_*
// Producer and consumer.
// Producer waits until has_value is 0.
// Consumer waits until has_value is 1.

int value;
int has_value;
int N = 5;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

typedef struct thread_arg {
    // Number of items currently processed.
    int curr;
} arg_t;

void *producer_run(void *arg)
{
    arg_t *a = (arg_t *) arg;

    while (a->curr < N) {
        pthread_mutex_lock(&mutex);

        while (has_value != 0) {
            // The thread atomically releases the mutex and blocks,
            // waiting for a notification on the condition variable.
            // When it wakes up, it reacquires the mutex before
            // pthread_cond_wait returns.
            pthread_cond_wait(&cond, &mutex);
        }

        // rand() isn't guaranteed to be thread-safe, but it is on Linux with glibc.
        int x = rand() % 10 + 1;

        value = x;
        has_value = 1;

        printf("Hello from producer: Produced %d.\n", x);

        // pthread_cond_signal wakes up one thread waiting on this condition variable.
        // With one producer and one consumer, only the other thread can be waiting.
        
        // Another option is to use pthread_cond_broadcast, which 
        // wakes up all threads waiting on this condition variable.

        // A thread that wakes up still needs to be scheduled and reacquire
        // the mutex before pthread_cond_wait returns.
        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&mutex);

        a->curr++;
    }

    return NULL;
}

void *consumer_run(void *arg)
{
    arg_t *a = (arg_t *) arg;

    while (a->curr < N) {
        pthread_mutex_lock(&mutex);

        while (has_value == 0) {
            pthread_cond_wait(&cond, &mutex);
        }

        printf("Hello from consumer: Consumed %d.\n", value);

        value = 0;
        has_value = 0;

        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);

        a->curr++;
    }

    return NULL;
}

int main()
{
    arg_t *arg = calloc(2, sizeof(arg_t));

    if (!arg) {
        fprintf(stderr, "Malloc failed.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t producer;

    int error = pthread_create(&producer, NULL, producer_run, &arg[0]);

    if (error) {
        fprintf(stderr, "pthread_create: %s\n", strerror(error));
        free(arg);
        exit(EXIT_FAILURE);
    }

    pthread_t consumer;

    error = pthread_create(&consumer, NULL, consumer_run, &arg[1]);

    if (error) {
        fprintf(stderr, "pthread_create: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    error = pthread_join(producer, NULL);

    if (error) {
        fprintf(stderr, "pthread_join: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    error = pthread_join(consumer, NULL);

    if (error) {
        fprintf(stderr, "pthread_join: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    free(arg);

    return 0;
}