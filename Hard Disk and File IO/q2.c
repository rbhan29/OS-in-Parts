#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#define TRACKS 5
#define SECTORS_PER_TRACK 100
#define ROTATIONAL_SPEED 72.0
#define SEEK_TIME_PER_TRACK 2.0
#define INITIAL_HEAD_POSITION 100

// Structure of disk request
typedef struct {
    int sector;
    int track;
} DiskRequest;

// Calculate track number from sector
int calculate_track(int sector) {
    return sector / SECTORS_PER_TRACK;
}

// Calculate absolute track distance
int track_distance(int current_track, int target_track) {
    return abs(current_track - target_track);
}

// Calculate seek time
double calculate_seek_time(int current_track, int target_track) {
    int track_diff = track_distance(current_track, target_track);
    return track_diff * SEEK_TIME_PER_TRACK;
}

// Calculate rotational latency
double calculate_rotational_latency(int initial_sector, int target_sector) {
    // One full rotation time
    double rotation_time = 60000.0 / ROTATIONAL_SPEED;  // ms
    
    // Calculate sector difference
    int sector_diff = abs(initial_sector - target_sector);
    
    // Calculate rotational latency based on sector difference
    double latency = (sector_diff / (double)SECTORS_PER_TRACK) * rotation_time;
    
    return latency;
}

// Find the nearest request from current head position
int find_nearest_request(DiskRequest* requests, int num_requests, int current_track, int* used_requests) {
    int nearest_index = -1;
    int min_distance = INT_MAX;

    for (int i = 0; i < num_requests; i++) {
        if (!used_requests[i]) {
            int target_track = calculate_track(requests[i].sector);
            int distance = track_distance(current_track, target_track);

            if (distance < min_distance) {
                min_distance = distance;
                nearest_index = i;
            }
        }
    }

    return nearest_index;
}

// Simulate SSTF
void simulate_sstf(DiskRequest* requests, int num_requests) {
    int current_track = calculate_track(INITIAL_HEAD_POSITION);
    int current_sector = INITIAL_HEAD_POSITION % SECTORS_PER_TRACK;
    int* used_requests = calloc(num_requests, sizeof(int));
    double total_seek_time = 0;
    double total_rotational_latency = 0;

    for (int i = 0; i < num_requests; i++) {
        // Find nearest request
        int request_index = find_nearest_request(requests, num_requests, current_track, used_requests);
        
        // Mark request as used
        used_requests[request_index] = 1;

        // Calculate target track and sector
        int target_track = calculate_track(requests[request_index].sector);
        int target_sector = requests[request_index].sector % SECTORS_PER_TRACK;

        // Calculate seek time
        double seek_time = calculate_seek_time(current_track, target_track);
        total_seek_time += seek_time;

        // Calculate rotational latency
        double rotational_latency = calculate_rotational_latency(current_sector, target_sector);
        total_rotational_latency += rotational_latency;

        // Output results for this request
        printf("Seek Time %d: %.2f ms\n", i+1, seek_time);
        printf("Rotational Latency %d: %.2f ms\n", i+1, rotational_latency);

        // Update current position
        current_track = target_track;
        current_sector = target_sector;
    }

    // Output total times
    printf("\nTotal Seek Time: %.2f ms\n", total_seek_time);
    printf("Total Rotational Latency: %.2f ms\n", total_rotational_latency);

    free(used_requests);
}

int main() {
    int num_requests;
    printf("Enter the number of disk requests: ");
    scanf("%d", &num_requests);

    DiskRequest* requests = malloc(num_requests * sizeof(DiskRequest));
    if (requests == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    for (int i = 0; i < num_requests; i++) {
        scanf("%d", &requests[i].sector);
        requests[i].track = calculate_track(requests[i].sector);
    }

    printf("Initial Head Position: %d\n", INITIAL_HEAD_POSITION);
    simulate_sstf(requests, num_requests);

    free(requests);
    return 0;
}
