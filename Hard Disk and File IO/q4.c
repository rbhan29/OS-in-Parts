#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void part1(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to open file");
        exit(1);
    }

    int saved_stdout = dup(STDOUT_FILENO);
    if (dup2(fd, STDOUT_FILENO) < 0) {
        perror("Failed to redirect stdout");
        close(fd);
        exit(1);
    }

    printf("This is some random text.\n");

    fflush(stdout);
    close(fd);

    if (dup2(saved_stdout, STDOUT_FILENO) < 0) {
        perror("Failed to restore stdout");
        exit(1);
    }
    close(saved_stdout);
}

void part2(const char *command, const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to open file");
        exit(1);
    }

    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stderr = dup(STDERR_FILENO);
    if (dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0) {
        perror("Failed to redirect stdout/stderr");
        close(fd);
        exit(1);
    }

    system(command);

    fflush(stdout);
    fflush(stderr);
    close(fd);

    if (dup2(saved_stdout, STDOUT_FILENO) < 0 || dup2(saved_stderr, STDERR_FILENO) < 0) {
        perror("Failed to restore stdout/stderr");
        exit(1);
    }
    close(saved_stdout);
    close(saved_stderr);
} 

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s -p1 <output_file> | -p2 <command> <output_file>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-p1") == 0) {
        part1(argv[2]);
    } else if (strcmp(argv[1], "-p2") == 0 && argc == 4) {
        part2(argv[2], argv[3]);
    } else {
        fprintf(stderr, "Invalid arguments\n");
        return 1;
    }

    return 0;
}
