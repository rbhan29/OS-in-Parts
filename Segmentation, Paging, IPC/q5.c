#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_FRAMES 100
#define MAX_REQUESTS 1000

struct page {
    uint8_t page_number;
    bool reference_bit;
    bool dirty_bit;
};

struct page memory[MAX_FRAMES];
int num_frames;
int clock_hand = 0;
int total_faults = 0;
int total_hits = 0;

void initialize_memory() {
    for (int i = 0; i < num_frames; i++) {
        memory[i].page_number = -1;
        memory[i].reference_bit = false;
        memory[i].dirty_bit = false;
    }
}
bool is_page_in_memory(uint8_t page_number) {
    for (int i = 0; i < num_frames; i++) {
        if (memory[i].page_number == page_number) {
            return true;
        }
    }
    return false;
}

int find_empty_frame() {
    for (int i = 0; i < num_frames; i++) {
        if (memory[i].page_number == -1) {
            return i;
        }
    }
    return -1;
}

int clock_algorithm() {
    int start_hand = clock_hand;
    while (true) {
        if (!memory[clock_hand].reference_bit) {
            if (!memory[clock_hand].dirty_bit) {
                int victim = clock_hand;
                clock_hand = (clock_hand + 1) % num_frames;
                return victim;
            } else {
                // Simulate writing back to disk
                memory[clock_hand].dirty_bit = false;
            }
        } else {
            memory[clock_hand].reference_bit = false;
        }
        clock_hand = (clock_hand + 1) % num_frames;
        if (clock_hand == start_hand) {
            // If we've gone full circle, replace the current page
            int victim = clock_hand;
            clock_hand = (clock_hand + 1) % num_frames;
            return victim;
        }
    }
}

void handle_page_request(uint8_t page_number) {
    if (is_page_in_memory(page_number)) {
        total_hits++;
        // Set reference bit
        for (int i = 0; i < num_frames; i++) {
            if (memory[i].page_number == page_number) {
                memory[i].reference_bit = true;
                break;
            }
        }
    } else {
        total_faults++;
        int frame = find_empty_frame();
        if (frame == -1) {
            frame = clock_algorithm();
        }
        memory[frame].page_number = page_number;
        memory[frame].reference_bit = true;
        memory[frame].dirty_bit = false;  // Assume newly loaded pages are not dirty
    }
}

int main() {
    char input[MAX_REQUESTS * 2];  // Assuming each request is at most 2 characters long
    printf("Enter number of frames: ");
    scanf("%d", &num_frames);
    getchar();  // Consume newline

    printf("Enter page requests (comma-separated): ");
    fgets(input, sizeof(input), stdin);

    initialize_memory();

    char *token = strtok(input, ",");
    while (token != NULL) {
        uint8_t page_number = atoi(token);
        handle_page_request(page_number);
        token = strtok(NULL, ",");
    }

    printf("Total page faults: %d\n", total_faults);
    printf("Total page hits: %d\n", total_hits);
    printf("Hit rate: %.2f%%\n", (float)total_hits / (total_hits + total_faults) * 100);

    return 0;
}
// 0,4,1,4,2,4,3,4,2,4,0,4,1,4,2,4,3,4 (f3,pf9,ph9)
// 2,3,2,1,5,2,4,5,3,2,5,2 (f3,pf8,ph4)
// 2,4,5,3,2,6,1,5,6,2 (f3,pf9,ph1)
