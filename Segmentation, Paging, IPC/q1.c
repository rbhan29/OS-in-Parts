#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 16

void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int leftArr[n1], rightArr[n2];

    for (int i = 0; i < n1; i++)
        leftArr[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        rightArr[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (leftArr[i] <= rightArr[j]) {
            arr[k] = leftArr[i];
            i++;
        } else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

void mergesort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // creating two pipes
        int pipefd1[2], pipefd2[2];

        if (pipe(pipefd1) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        if (pipe(pipefd2) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid1 = fork();
        if (pid1 == -1) {
            // fork failed
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid1 == 0) {
            // child process
            close(pipefd1[0]);
            mergesort(arr, left, mid);
            // writing the left half of the array to the pipe
            if (write(pipefd1[1], arr + left, (mid - left + 1) * sizeof(int)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            close(pipefd1[1]);
            exit(0);
        }

        pid_t pid2 = fork();
        if (pid2 == -1) {
            // fork failed
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid2 == 0) {
            // child process
            close(pipefd2[0]);
            mergesort(arr, mid + 1, right);
            // writing the right half of the array to the pipe
            if (write(pipefd2[1], arr + mid + 1, (right - mid) * sizeof(int)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            close(pipefd2[1]);
            exit(0);
        }

        // waiting for both child processes to finish
        wait(NULL);
        wait(NULL);

        close(pipefd1[1]);
        close(pipefd2[1]);
        if (read(pipefd1[0], arr + left, (mid - left + 1) * sizeof(int)) == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (read(pipefd2[0], arr + mid + 1, (right - mid) * sizeof(int)) == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        close(pipefd1[0]);
        close(pipefd2[0]);

        merge(arr, left, mid, right);
    }
}

int main() {
    int arr[SIZE] = {6,3,2,8,12,5,7,16,9,10,11,1,15,14,13,4};

    printf("Unsorted array: ");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    mergesort(arr, 0, SIZE - 1);

    printf("Sorted array:   ");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
