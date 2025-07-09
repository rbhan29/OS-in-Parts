#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int count_words(FILE *file) {
    int count = 0;
    char c;
    int in_word = 0;

    while ((c = fgetc(file)) != EOF) {
        if (isspace(c)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
    }

    return count;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    int word_count = count_words(file);
    printf("Word count: %d\n", word_count);

    fclose(file);
    return 0;
}
