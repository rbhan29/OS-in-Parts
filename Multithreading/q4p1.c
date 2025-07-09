#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int row;
    int col;
    int n;
    int** A;
    int** B;
    int** C;
} ThreadData;

void* compute_element(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int sum = 0;
    for (int k = 0; k < data->n; k++) {
        sum += data->A[data->row][k] * data->B[k][data->col];
    }
    data->C[data->row][data->col] = sum;
    free(data);
    return NULL;
}

int main() {
    int m, n, p;

    printf("Enter dimensions of Matrix A (m x n): ");
    scanf("%d %d", &m, &n);
    printf("Enter dimensions of Matrix B (n x p): ");
    scanf("%d %d", &n, &p);

    int** A = (int**)malloc(m * sizeof(int*));
    int** B = (int**)malloc(n * sizeof(int*));
    int** C = (int**)malloc(m * sizeof(int*));
    for (int i = 0; i < m; i++) A[i] = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) B[i] = (int*)malloc(p * sizeof(int));
    for (int i = 0; i < m; i++) C[i] = (int*)malloc(p * sizeof(int));

    printf("Enter elements of Matrix A:\n");
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            scanf("%d", &A[i][j]);

    printf("Enter elements of Matrix B:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < p; j++)
            scanf("%d", &B[i][j]);

    clock_t start = clock();

    pthread_t threads[m * p];
    int thread_index = 0;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
            data->row = i;
            data->col = j;
            data->n = n;
            data->A = A;
            data->B = B;
            data->C = C;
            pthread_create(&threads[thread_index++], NULL, compute_element, data);
        }
    }

    for (int i = 0; i < m * p; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end = clock();
    double time_parallel = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Resultant Matrix C:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    start = clock();
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            int sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    end = clock();
    double time_sequential = (double)(end - start) / CLOCKS_PER_SEC;

    printf("time parallel is %f\n", time_parallel);
    printf("Speedup over sequential execution: %f\n", time_sequential / time_parallel);

    for (int i = 0; i < m; i++) free(A[i]);
    for (int i = 0; i < n; i++) free(B[i]);
    for (int i = 0; i < m; i++) free(C[i]);
    free(A);
    free(B);
    free(C);

    return 0;
}
