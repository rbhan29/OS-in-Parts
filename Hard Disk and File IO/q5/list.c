#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

void list_directory(const char *path) {
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp))) {
        printf("%s\n", entry->d_name);
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
    const char *path = (argc > 1) ? argv[1] : ".";
    list_directory(path);
    return 0;
}
