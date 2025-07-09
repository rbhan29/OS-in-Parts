#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int row;
    int col;
    int n;
    int** A;
    int** B;
    int** C;
} Task;

typedef struct {
    Task** task_queue;
    int queue_size;
    int front;
    int rear;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} ThreadPool;

ThreadPool pool;
pthread_t* workers;
int num_cores;

void* worker_function(void* arg) {
    while (1) {
        pthread_mutex_lock(&pool.lock);

        while (pool.front == pool.rear) { 
            pthread_cond_wait(&pool.cond, &pool.lock);
        }

        Task* task = pool.task_queue[pool.front];
        pool.front = (pool.front + 1) % pool.queue_size;

        pthread_mutex_unlock(&pool.lock);

        if (task != NULL) {
            int sum = 0;
            for (int k = 0; k < task->n; k++) {
                sum += task->A[task->row][k] * task->B[k][task->col];
            }
            task->C[task->row][task->col] = sum;
            free(task); 
        }
    }
    return NULL;
}

void initialize_threadpool(int cores, int max_tasks) {
    num_cores = cores;
    pool.task_queue = (Task**)malloc(max_tasks * sizeof(Task*));
    pool.queue_size = max_tasks;
    pool.front = pool.rear = 0;
    pthread_mutex_init(&pool.lock, NULL);
    pthread_cond_init(&pool.cond, NULL);

    workers = (pthread_t*)malloc(cores * sizeof(pthread_t));
    for (int i = 0; i < cores; i++) {
        pthread_create(&workers[i], NULL, worker_function, NULL);
    }
}

void add_task(Task* task) {
    pthread_mutex_lock(&pool.lock);

    pool.task_queue[pool.rear] = task;
    pool.rear = (pool.rear + 1) % pool.queue_size;

    pthread_cond_signal(&pool.cond);
    pthread_mutex_unlock(&pool.lock);
}

void sequential_matrix_multiplication(int** A, int** B, int** C, int m, int n, int p) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void* parallel_worker(void* arg) {
    Task* task = (Task*)arg;
    int sum = 0;
    for (int k = 0; k < task->n; k++) {
        sum += task->A[task->row][k] * task->B[k][task->col];
    }
    task->C[task->row][task->col] = sum;
    free(task);
    return NULL;
}

void parallel_matrix_multiplication(int** A, int** B, int** C, int m, int n, int p) {
    pthread_t threads[m * p];
    int thread_index = 0;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            Task* task = (Task*)malloc(sizeof(Task));
            task->row = i;
            task->col = j;
            task->n = n;
            task->A = A;
            task->B = B;
            task->C = C;
            pthread_create(&threads[thread_index++], NULL, parallel_worker, task);
        }
    }

    for (int i = 0; i < thread_index; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main() {
    int m, n, p;
    printf("Enter dimensions of Matrix A (m x n): ");
    scanf("%d %d", &m, &n);
    printf("Enter dimensions of Matrix B (n x p): ");
    scanf("%d %d", &n, &p);

    int** A = (int**)malloc(m * sizeof(int*));
    int** B = (int**)malloc(n * sizeof(int*));
    int** C_parallel = (int**)malloc(m * sizeof(int*));
    int** C_sequential = (int**)malloc(m * sizeof(int*));
    int** C_threadpool = (int**)malloc(m * sizeof(int*));
    for (int i = 0; i < m; i++) A[i] = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) B[i] = (int*)malloc(p * sizeof(int));
    for (int i = 0; i < m; i++) {
        C_parallel[i] = (int*)malloc(p * sizeof(int));
        C_sequential[i] = (int*)malloc(p * sizeof(int));
        C_threadpool[i] = (int*)malloc(p * sizeof(int));
    }

    printf("Enter elements of Matrix A:\n");
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            scanf("%d", &A[i][j]);

    printf("Enter elements of Matrix B:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < p; j++)
            scanf("%d", &B[i][j]);

    int max_tasks = m * p;
    initialize_threadpool(sysconf(_SC_NPROCESSORS_ONLN), max_tasks);

    clock_t start_sequential = clock();
    sequential_matrix_multiplication(A, B, C_sequential, m, n, p);
    clock_t end_sequential = clock();
    double time_sequential = (double)(end_sequential - start_sequential) / CLOCKS_PER_SEC;

    clock_t start_parallel = clock();
    parallel_matrix_multiplication(A, B, C_parallel, m, n, p);
    clock_t end_parallel = clock();
    double time_parallel = (double)(end_parallel - start_parallel) / CLOCKS_PER_SEC;

    clock_t start_threadpool = clock();

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            Task* task = (Task*)malloc(sizeof(Task));
            task->row = i;
            task->col = j;
            task->n = n;
            task->A = A;
            task->B = B;
            task->C = C_threadpool;
            add_task(task);
        }
    }

    sleep(1);

    clock_t end_threadpool = clock();
    double time_threadpool = (double)(end_threadpool - start_threadpool) / CLOCKS_PER_SEC;

    double speedup_over_sequential = time_sequential / time_threadpool;
    double speedup_over_parallel = time_parallel / time_threadpool;

    printf("\nResultant Matrix C :\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            printf("%d ", C_sequential[i][j]);
        }
        printf("\n");
    }

    printf("\nSequential Execution Time: %f seconds\n", time_sequential);
    printf("Parallel Execution Time without Thread Pool: %f seconds\n", time_parallel);
    printf("Parallel Execution Time with Thread Pool: %f seconds\n", time_threadpool);
    printf("Speedup over Sequential: %f\n", speedup_over_sequential);
    printf("Speedup over Parallel without Thread Pool: %f\n", speedup_over_parallel);

    for (int i = 0; i < m; i++) {
        free(A[i]);
        free(C_parallel[i]);
        free(C_sequential[i]);
        free(C_threadpool[i]);
    }
    for (int i = 0; i < n; i++) free(B[i]);
    free(A);
    free(B);
    free(C_parallel);
    free(C_sequential);
    free(C_threadpool);
    free(workers);

    return 0;
}