#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h> // For intptr_t

#define BUFFER_SIZE 25 
#define DELIVERY_MAX_PRODUCTS 6 
#define STORAGE_MAX_PRODUCTS 3 
#define MAX_DELIVERIES 2 
int buffer[BUFFER_SIZE];
int count = 0;  
int in = 0;     
int out = 0;    

sem_t empty;    
sem_t full;     
pthread_mutex_t mutex;  

int *deliveries; 

void* delivery_truck(void* arg) {
    int id = (intptr_t)arg;
    while (1) {
        int products = rand() % DELIVERY_MAX_PRODUCTS + 1; 
        
        printf("Truck %d Delivering %d Products.\n", id, products);

        while (products > 0) {
            //Checking if the buffer is full
            if (count == BUFFER_SIZE) {
                printf("Truck %d is waiting, warehouse is full!\n", id);
            }

            sem_wait(&empty);  //Waiting for at least one space
            pthread_mutex_lock(&mutex);

            // Delivering one product at a time
            buffer[in] = 1;
            in = (in + 1) % BUFFER_SIZE;
            count++;
            products--;  //Reducing the remaining products to deliver
            printf("Truck %d delivered 1 product, Products Remaining to deliver: %d. Current inventory is : %d\n", id, products, count);

            pthread_mutex_unlock(&mutex);
            sem_post(&full);  //Signaling that a product is available
        }

        deliveries[id]++;
        if (deliveries[id] >= MAX_DELIVERIES) {
            printf("Truck %d has completed %d deliveries, Exited.\n", id, MAX_DELIVERIES);
            pthread_exit(NULL);
        }

        sleep(1);  
    }
    return NULL;
}

void* storage_manager(void* arg) {
    int id = (intptr_t)arg;
    while (1) {
        int products = rand() % STORAGE_MAX_PRODUCTS + 1;  
        
        sem_wait(&full);  //Waiting if there are no filled spaces
        pthread_mutex_lock(&mutex);  //Locking the buffer
        
        //Removing products from the buffer
        if (count > 0) {
            for (int i = 0; i < products && count > 0; i++) {
                buffer[out] = 0;  
                out = (out + 1) % BUFFER_SIZE;
                count--;
            }

            printf("Storage Manager %d stored %d products. Current inventory is: %d\n", id, products, count);
        } else {
            printf("Storage Manager %d waiting, warehouse is empty\n", id);
        }
        
        pthread_mutex_unlock(&mutex);  
        sem_post(&empty);  

        sleep(1);  
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <num_trucks> <num_managers>\n", argv[0]);
        exit(1);
    }

    int num_trucks = atoi(argv[1]);
    int num_managers = atoi(argv[2]);
    pthread_t truck_threads[num_trucks], manager_threads[num_managers];
    int truck_ids[num_trucks], manager_ids[num_managers];

    deliveries = (int*)malloc(num_trucks * sizeof(int));
    if (deliveries == NULL) {
        perror("Failed to allocate memory for deliveries array");
        exit(1);
    }
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < num_trucks; i++) {
        deliveries[i] = 0;
    }

    for (int i = 0; i < num_trucks; i++) {
        truck_ids[i] = i + 1;
        pthread_create(&truck_threads[i], NULL, delivery_truck, (void*)(intptr_t)truck_ids[i]);
    }

    for (int i = 0; i < num_managers; i++) {
        manager_ids[i] = i + 1;
        pthread_create(&manager_threads[i], NULL, storage_manager, (void*)(intptr_t)manager_ids[i]);
    }

    for (int i = 0; i < num_trucks; i++) {
        pthread_join(truck_threads[i], NULL);
    }

    for (int i = 0; i < num_managers; i++) {
        pthread_cancel(manager_threads[i]);
        pthread_join(manager_threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    printf("Final Inventory is: %d\n", count);

    free(deliveries);

    return 0;
}