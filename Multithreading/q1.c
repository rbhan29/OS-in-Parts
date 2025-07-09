#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

// Lock A and Lock B
pthread_mutex_t lockA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockB = PTHREAD_MUTEX_INITIALIZER;

void* thread_function(void* arg) {
    int thread_id = *(int*)arg;
    int count = 3; 

    while (count > 0) {
        // Acquiring Lock A
        if (pthread_mutex_trylock(&lockA) == 0) {
            printf("T%d acquired Lock A\n", thread_id);

            // Acquiring Lock B
            if (pthread_mutex_trylock(&lockB) == 0) {
                printf("T%d acquired Lock B\n", thread_id);

                // Releasing Lock B 
                pthread_mutex_unlock(&lockB);
            } else {
                printf("T%d waiting for Lock B\n", thread_id);
            }

            // Releasing Lock A 
            pthread_mutex_unlock(&lockA);
        } else {
            printf("T%d waiting for Lock A\n", thread_id);
        }

        usleep(1000);

        count--;
    }

    free(arg);
    return NULL;
}

int main() {
    pthread_t threads[3];

    for (int i = 0; i < 3; i++) {
        int* thread_id = malloc(sizeof(int));
        *thread_id = i + 1; // Assign thread ID starting from 1
        pthread_create(&threads[i], NULL, thread_function, thread_id);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lockA);
    pthread_mutex_destroy(&lockB);

    return 0;
}
