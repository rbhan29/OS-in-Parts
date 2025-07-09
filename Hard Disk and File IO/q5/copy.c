#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

void copy_file(const char *src, const char *dest) {
    int src_fd = open(src, O_RDONLY);
    if (src_fd < 0) {
        perror("open src");
        return;
    }

    struct stat st;
    if (stat(dest, &st) == 0 && S_ISDIR(st.st_mode)) {
        char *filename = strrchr(src, '/');
        filename = (filename) ? filename + 1 : (char *)src;
        char dest_path[1024];
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, filename);
        dest = dest_path;
    }

    int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        perror("open dest");
        close(src_fd);
        return;
    }

    char buffer[1024];
    ssize_t bytes;
    while ((bytes = read(src_fd, buffer, sizeof(buffer))) > 0) {
        write(dest_fd, buffer, bytes);
    }

    close(src_fd);
    close(dest_fd);
    printf("%s copied to %s\n", src, dest);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }

    copy_file(argv[1], argv[2]);
    return 0;
}
