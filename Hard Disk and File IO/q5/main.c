#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void run_program(const char *program, char *const argv[]) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(program, argv);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("fork");
    }
}

int main() {
    char *list_args[] = {"./list", "..", NULL};
    char *countw_args[] = {"./countw", "Makefile", NULL};
    char *copy_args[] = {"./copy", "list.c", "..", NULL};
    char *move_args[] = {"./move", ".", "../q5_files", NULL};

    printf("\nRunning list...\n");
    run_program("./list", list_args);

    printf("\nRunning countw...\n");
    run_program("./countw", countw_args);

    printf("\nRunning copy...\n");
    run_program("./copy", copy_args);

    printf("\nRunning move...\n");
    run_program("./move", move_args);

    return 0;
}
