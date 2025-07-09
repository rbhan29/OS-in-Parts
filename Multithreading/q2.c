#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TOTAL_SERVERS 5
#define MAX_TASKS 3

sem_t channels[TOTAL_SERVERS];

void* server_func(void* arg);

void* server_func(void* arg) {
    int server_id = *(int*)arg;
    int left_channel = server_id;
    int rght_channel = (server_id + 1) % TOTAL_SERVERS;

    for (int task_count = 0; task_count < MAX_TASKS; task_count++) {
        printf("Server %d is waiting for channels %d and %d\n", server_id + 1, left_channel + 1, rght_channel + 1);

        if (left_channel < rght_channel) {
            sem_wait(&channels[left_channel]);
            sem_wait(&channels[rght_channel]);
        } else {
            sem_wait(&channels[rght_channel]);
            sem_wait(&channels[left_channel]);
        }

        printf("Server %d is processing\n", server_id + 1);
        sleep(1);

        sem_post(&channels[left_channel]);
        sem_post(&channels[rght_channel]);

        printf("Server %d has finished processing\n", server_id + 1);
    }

    return NULL;
}


int main() {
    pthread_t threads[TOTAL_SERVERS];
    int ids[TOTAL_SERVERS];

    for (int i = 0; i < TOTAL_SERVERS; i++) {
        sem_init(&channels[i], 0, 1);
    }

    for (int i = 0; i < TOTAL_SERVERS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, server_func, &ids[i]);
    }

    for (int i = 0; i < TOTAL_SERVERS; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < TOTAL_SERVERS; i++) {
        sem_destroy(&channels[i]);
    }

    return 0;

}
