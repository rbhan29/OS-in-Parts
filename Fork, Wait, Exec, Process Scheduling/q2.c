#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void binary_search(int arr[], int start, int end, int target) {
    if (start > end) {
        printf("-1\n");
        exit(0);
    }

    int mid = start + (end - start) / 2;

    if (arr[mid] == target) {
        printf("Target value's index: %d\n", mid);
        exit(0);
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(1);
    } else if (pid == 0) {
        if (arr[mid] < target) {
            binary_search(arr, mid + 1, end, target);
        } else {
            binary_search(arr, start, mid - 1, target);
        }
    } else {
        // Parent waits for each child to finish
        wait(NULL);
        exit(0);
    }
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    int n = sizeof(arr) / sizeof(arr[0]);
    int target;

    printf("Array: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\nEnter a target value: ");
    scanf("%d", &target);

    binary_search(arr, 0, n - 1, target);

    return 0;
}
