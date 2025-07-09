#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void generate_and_print_average() {
    int sum = 0;
    for (int i = 0; i < 4; i++) {
        int random_num = rand() % 100 + 1; 
        sum += random_num;
    }
    float average = sum / 4.0; 
    printf("Child (pid: %d) Average: %.1f\n", (int)getpid(), average);
}

int main() {
    printf("Parent(pid: %d)\n", (int)getpid());
    fflush(stdout); 

    for (int i = 0; i < 7; i++) {
        int rc = fork(); 
        if (rc < 0) {    
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) { 
            srand(time(NULL) + getpid()); // Seeding using time and pid to ensure that each child is unique to avoid having same random numbers
            generate_and_print_average();
            exit(0); // child terminates after printing the average
        }
    }

    // Parent waits for each child to finish
    for (int i = 0; i < 7; i++) {
        wait(NULL);
    }
    return 0;
}
