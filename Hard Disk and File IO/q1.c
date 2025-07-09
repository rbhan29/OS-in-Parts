#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <bits/mman-linux.h>


#define MAX_STATIONS 10000
#define MAX_NAME_LENGTH 100

// Station data structure
typedef struct {
    char name[MAX_NAME_LENGTH];
    double min_temp;
    double max_temp;
    double total_temp;
    int count;
} StationData;

// Function to compare stations for sorting
int compare_stations(const void* a, const void* b) {
    return strcmp(((StationData*)a)->name, ((StationData*)b)->name);
}

// Function to find or add a station
StationData* find_or_add_station(StationData* stations, int* station_count, const char* name) {
    for (int i = 0; i < *station_count; i++) {
        if (strcmp(stations[i].name, name) == 0) {
            return &stations[i];
        }
    }
    
    // If not found, add new station
    if (*station_count < MAX_STATIONS) {
        StationData* new_station = &stations[*station_count];
        strncpy(new_station->name, name, MAX_NAME_LENGTH - 1);
        new_station->min_temp = DBL_MAX;
        new_station->max_temp = -DBL_MAX;
        new_station->total_temp = 0;
        new_station->count = 0;
        (*station_count)++;
        return new_station;
    }
    
    return NULL;
}

// Using memory-mapped file (mmap)
void process_file_mmap(const char* filename) {
    // Open the file
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    // Get file size
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Error getting file size");
        close(fd);
        return;
    }

    // Memory map the file
    char* file_contents = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_contents == MAP_FAILED) {
        perror("Error memory mapping file");
        close(fd);
        return;
    }

    madvise(file_contents, sb.st_size, MADV_SEQUENTIAL);

    // Station data storage
    StationData stations[MAX_STATIONS];
    int station_count = 0;

    // Process file contents
    char* line = file_contents;
    char* end = file_contents + sb.st_size;
    
    while (line < end) {
        // Find end of current line
        char* next_line = memchr(line, '\n', end - line);
        if (!next_line) next_line = end;
        
        // Null-terminate the line temporarily
        char line_copy[MAX_NAME_LENGTH + 20];
        memcpy(line_copy, line, next_line - line);
        line_copy[next_line - line] = '\0';
        
        // Parse station name and temperature
        char* semicolon = strchr(line_copy, ';');
        if (semicolon) {
            *semicolon = '\0';
            double temperature = atof(semicolon + 1);
            
            // Find or add station
            StationData* station = find_or_add_station(stations, &station_count, line_copy);
            if (station) {
                station->min_temp = (temperature < station->min_temp) ? temperature : station->min_temp;
                station->max_temp = (temperature > station->max_temp) ? temperature : station->max_temp;
                station->total_temp += temperature;
                station->count++;
            }
        }
        
        // Move to next line
        line = next_line + 1;
    }

    // Sort and print results
    qsort(stations, station_count, sizeof(StationData), compare_stations);
    for (int i = 0; i < station_count; i++) {
        double mean = stations[i].total_temp / stations[i].count;
        printf("%s min=%.1f mean=%.1f max=%.1f\n", 
               stations[i].name, 
               stations[i].min_temp, 
               mean, 
               stations[i].max_temp);
    }

    // Cleanup
    munmap(file_contents, sb.st_size);
    close(fd);
}

// Using standard file reading (fread)
void process_file_fread(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Station data storage
    StationData stations[MAX_STATIONS];
    int station_count = 0;

    // Buffer for reading lines
    char line[MAX_NAME_LENGTH + 20];
    
    // Process file line by line
    while (fgets(line, sizeof(line), file)) {
        // Remove newline if present
        line[strcspn(line, "\n")] = 0;
        
        // Parse station name and temperature
        char* semicolon = strchr(line, ';');
        if (semicolon) {
            *semicolon = '\0';
            double temperature = atof(semicolon + 1);
            
            // Find or add station
            StationData* station = find_or_add_station(stations, &station_count, line);
            if (station) {
                station->min_temp = (temperature < station->min_temp) ? temperature : station->min_temp;
                station->max_temp = (temperature > station->max_temp) ? temperature : station->max_temp;
                station->total_temp += temperature;
                station->count++;
            }
        }
    }

    // Sort and print results
    qsort(stations, station_count, sizeof(StationData), compare_stations);
    for (int i = 0; i < station_count; i++) {
        double mean = stations[i].total_temp / stations[i].count;
        printf("%s min=%.1f mean=%.1f max=%.1f\n", 
               stations[i].name, 
               stations[i].min_temp, 
               mean, 
               stations[i].max_temp);
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    const char* filename = "q1-50mil.txt";

    clock_t start, end;
    double cpu_time_used;

    // Process using mmap
    start = clock();
    process_file_mmap(filename);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nTime taken using mmap: %f seconds\n", cpu_time_used);
    double mmap_time = cpu_time_used;

    printf("\n-------------------------------------------------------------------\n");

    // Process using fread
    start = clock();
    process_file_fread(filename);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nTime taken using fread: %f seconds\n", cpu_time_used);
    double fread_time = cpu_time_used;

    printf("\n-------------------------------------------------------------------\n");

    printf("\nTime taken using mmap: %f seconds\n", mmap_time);
    printf("\nTime taken using fread: %f seconds\n", fread_time);

    return 0;
}
