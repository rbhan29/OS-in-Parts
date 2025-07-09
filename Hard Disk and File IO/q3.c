#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define FILENAME "students.rec"
#define RECORD_SIZE sizeof(struct Student)

struct Student {
    int id;         // Incremental student ID (4 bytes)
    char name[20];  // Student name (20 bytes)
};

int main() {
    int student_id;
    printf("Enter student id (1-5): ");
    scanf("%d", &student_id);

    if (student_id < 1 || student_id > 5) {
        printf("Invalid student ID. Please enter a value between 1 and 5.\n");
        return 1;
    }

    int fd = open(FILENAME, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    // Calculate the offset for the given student ID
    off_t offset = (student_id - 1) * RECORD_SIZE;

    // Use lseek to move to the correct position
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking in file");
        close(fd);
        return 1;
    }

    struct Student student;
    if (read(fd, &student, RECORD_SIZE) != RECORD_SIZE) {
        perror("Error reading from file");
        close(fd);
        return 1;
    }

    printf("Student name: %s\n", student.name);

    close(fd);
    return 0;
}
