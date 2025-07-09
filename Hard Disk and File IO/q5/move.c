#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

void move_file(const char *src, const char *dest) {
    if (rename(src, dest) != 0) {
        perror("rename");
    }
}

void move_directory(const char *src, const char *dest) {
    struct stat st;
    if (stat(dest, &st) != 0) {
        mkdir(dest, 0755);
    }

    DIR *dir = opendir(src);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_path[1024];
        char dest_path[1024];
        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        if (entry->d_type == DT_DIR) {
            move_directory(src_path, dest_path);
        } else {
            move_file(src_path, dest_path);
        }
    }

    closedir(dir);
    rmdir(src);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }

    move_directory(argv[1], argv[2]);
    return 0;
}
