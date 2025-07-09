#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

struct process_t {
    int pid;              // Process ID
    int arrivalTime;      // Arrival Time
    int burstTime;        // Burst Time
    int startTime;        // Start Time
    int completionTime;   // Completion Time
    int turnaroundTime;   // Turnaround Time
    int responseTime;     // Response Time
    int remainingTime;    // Remaining Time for RR and SRTF
    bool started;         // Process has started or not
};


void input_processes(struct process_t processes[], int n) {
    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1; // Assign PID starting from 1
        printf("Enter arrival time and burst time for process %d: ", i + 1);
        scanf("%d %d", &processes[i].arrivalTime, &processes[i].burstTime);
        processes[i].remainingTime = processes[i].burstTime; // Initializing remaining time
        processes[i].started = false; // Initially, no process has started
    }
}

// Function for FIFO Scheduling
void fifo(struct process_t process[], int n) {
    // Sorting based on Arrival Time (if equal, sort by PID)
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (process[j].arrivalTime > process[j + 1].arrivalTime ||
               (process[j].arrivalTime == process[j + 1].arrivalTime && process[j].pid > process[j + 1].pid)) {
                struct process_t temp = process[j];
                process[j] = process[j + 1];
                process[j + 1] = temp;
            }
        }
    }

    int currTime = 0; // Current time in the scheduling
    float total_turnaround_time = 0, total_response_time = 0; // Variables to store total turnaround and response times

    printf("\nFIFO Scheduling\n");
    printf("Order of Execution: ");
    
    // Process execution in FIFO order
    for (int i = 0; i < n; i++) {
        if (currTime < process[i].arrivalTime) {
            currTime = process[i].arrivalTime; // Adjust current time if needed
        }
        process[i].startTime = currTime; // Set start time of the process
        process[i].completionTime = currTime + process[i].burstTime; // Calculating completion time
        process[i].turnaroundTime = process[i].completionTime - process[i].arrivalTime; // Calculating turnaround time
        process[i].responseTime = process[i].startTime - process[i].arrivalTime; // Calculating response time

        total_turnaround_time += process[i].turnaroundTime; // Accumulating turnaround time
        total_response_time += process[i].responseTime; // Accumulating response time

        printf("%d ", process[i].pid); // Printing process PID

        currTime += process[i].burstTime; // Moving current time forward by the burst time of the process
    }

    printf("\nAverage Response Time: %.2f\n", total_response_time / n); 
    printf("Average Turnaround Time: %.2f\n", total_turnaround_time / n); 
}

// Function for SJF Scheduling
void sjf(struct process_t p[], int n) {
    int completed = 0, current_time = 0, total_turnaround_time = 0, total_response_time = 0;
    int min_index; // Index of the process with minimum burst time
    int is_completed[n]; // Array to track completed processes

    // Initializing is_completed array to track which processes are completed
    for (int i = 0; i < n; i++) {
        is_completed[i] = 0;
    }

    printf("\nSJF Scheduling\n");
    printf("Order of Execution: ");
    
    // Process execution in Shortest Job First (SJF) order
    while (completed != n) {
        min_index = -1; // Initializing minimum index
        int min_burst_time = INT_MAX; // Initializing minimum burst time

        // Find process with minimum burst time that is yet to be completed
        for (int i = 0; i < n; i++) {
            if (p[i].arrivalTime <= current_time && !is_completed[i]) {
                if (p[i].burstTime < min_burst_time) {
                    min_burst_time = p[i].burstTime;
                    min_index = i;
                }
                // If burst times are equal, choose the one with earlier arrival time
                if (p[i].burstTime == min_burst_time && p[i].arrivalTime < p[min_index].arrivalTime) {
                    min_index = i;
                }
            }
        }

        if (min_index != -1) {
            p[min_index].startTime = current_time; // Set start time
            p[min_index].completionTime = current_time + p[min_index].burstTime; // Calculating completion time
            p[min_index].turnaroundTime = p[min_index].completionTime - p[min_index].arrivalTime; // Calculate turnaround time
            p[min_index].responseTime = p[min_index].startTime - p[min_index].arrivalTime; // Calculate response time

            total_response_time += p[min_index].responseTime; // Accumulate response time
            total_turnaround_time += p[min_index].turnaroundTime; // Accumulate turnaround time

            printf("%d ", p[min_index].pid); // Print process PID

            current_time += p[min_index].burstTime; // Move current time forward by the burst time of the process
            is_completed[min_index] = 1; // Mark process as completed
            completed++; // Increment completed process count
        } else {
            current_time++; // Move current time forward if no process is ready to execute
        }
    }

    printf("\nAverage Response Time: %.2f\n", (float)total_response_time / n); 
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time / n); 
}

// Function for SRTF Scheduling
void srtf(struct process_t p[], int n) {
    int burst_remaining[n], is_completed[n], completed = 0;
    int current_time = 0, total_turnaround_time = 0, total_response_time = 0;
    memset(is_completed, 0, sizeof(is_completed)); // Initializing is_completed array

    // Initializing burst remaining times
    for (int i = 0; i < n; i++) {
        burst_remaining[i] = p[i].burstTime;
    }

    printf("\nSRTF Scheduling\n");
    printf("Order of Execution: ");

    // Process execution in Shortest Remaining Time First (SRTF) order
    while (completed != n) {
        int idx = -1, min_burst_time = INT_MAX; // Initialize index and minimum burst time

        // Find process with minimum remaining burst time
        for (int i = 0; i < n; i++) {
            if (p[i].arrivalTime <= current_time && !is_completed[i]) {
                if (burst_remaining[i] < min_burst_time) {
                    min_burst_time = burst_remaining[i];
                    idx = i;
                }
                // If remaining burst times are equal, choose the one with earlier arrival time
                if (burst_remaining[i] == min_burst_time && p[i].arrivalTime < p[idx].arrivalTime) {
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            if (burst_remaining[idx] == p[idx].burstTime) {
                p[idx].startTime = current_time; // Marking start time for the process
            }

            burst_remaining[idx]--; // Decrementing the remaining burst time
            current_time++; // Move current time forward

            if (burst_remaining[idx] == 0) {
                p[idx].completionTime = current_time; // Set completion time
                p[idx].turnaroundTime = p[idx].completionTime - p[idx].arrivalTime; // Calculate turnaround time
                p[idx].responseTime = p[idx].startTime - p[idx].arrivalTime; // Calculate response time

                total_turnaround_time += p[idx].turnaroundTime; // Accumulate turnaround time
                total_response_time += p[idx].responseTime; // Accumulate response time

                is_completed[idx] = 1; // Mark process as completed
                completed++; // Increment completed process count
                printf("%d ", p[idx].pid); // Print process PID
            }
        } else {
            current_time++; // Moving current time forward if no process is ready to execute
        }
    }

    printf("\nAverage Response Time: %.2f\n", (float)total_response_time / n); 
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time / n); 
}

// Function for Round Robin (RR) Scheduling
void rr(struct process_t p[], int n, int timeQuantum) {
    int current_time = 0, completed = 0, total_turnaround_time = 0, total_response_time = 0;
    int idx = 0; // Index for the process queue
    int is_completed[n], queue[n]; // Arrays for tracking completed processes and queue

    memset(is_completed, 0, sizeof(is_completed)); // Initializing is_completed array
    memset(queue, 0, sizeof(queue)); // Initializing queue array

    printf("\nRound Robin Scheduling\n");
    printf("Order of Execution: ");

    // Initializing the queue with processes
    for (int i = 0; i < n; i++) {
        queue[i] = p[i].pid;
    }

    // Round Robin Scheduling
    while (completed < n) {
        if (idx >= n) {
            idx = 0; // Reset index if it goes out of bounds
        }

        int process_id = queue[idx] - 1; // Get the process ID and convert to index
        if (p[process_id].arrivalTime <= current_time && !is_completed[process_id]) {
            if (!p[process_id].started) {
                p[process_id].startTime = current_time; // Set start time
                p[process_id].started = true; // Mark process as started
            }

            // Calculate execution time and update remaining time
            int execution_time = (p[process_id].remainingTime > timeQuantum) ? timeQuantum : p[process_id].remainingTime;
            p[process_id].remainingTime -= execution_time; // Updating remaining time
            current_time += execution_time; // Move current time forward

            if (p[process_id].remainingTime == 0) {
                p[process_id].completionTime = current_time; // Set completion time
                p[process_id].turnaroundTime = p[process_id].completionTime - p[process_id].arrivalTime; // Calculate turnaround time
                p[process_id].responseTime = p[process_id].startTime - p[process_id].arrivalTime; // Calculate response time

                total_turnaround_time += p[process_id].turnaroundTime; // Accumulate turnaround time
                total_response_time += p[process_id].responseTime; // Accumulate response time

                is_completed[process_id] = 1; // Mark process as completed
                completed++; // Increment completed process count
            }

            printf("%d ", p[process_id].pid); 
        }

        idx++; // Move to the next process in the queue
    }

    printf("\nAverage Response Time: %.2f\n", (float)total_response_time / n); 
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time / n); 
}


int main() {
    int n; 
    int timeQuantum; 
    char algo[4]; 
    
    
    printf("Enter number of processes: ");
    scanf("%d", &n);

    
    struct process_t processes[n];
    input_processes(processes, n); 

    // Input algorithm choice
    printf("Enter scheduling algorithm (FIFO/SJF/SRTF/RR): ");
    scanf("%s", algo);

    // Input time quantum for Round Robin
    if (strcmp(algo, "RR") == 0) {
        printf("Enter time quantum: ");
        scanf("%d", &timeQuantum);
    }

    if (strcmp(algo, "FIFO") == 0) {
        fifo(processes, n);
    } else if (strcmp(algo, "SJF") == 0) {
        sjf(processes, n);
    } else if (strcmp(algo, "SRTF") == 0) {
        srtf(processes, n);
    } else if (strcmp(algo, "RR") == 0) {
        rr(processes, n, timeQuantum);
    } else {
        printf("Invalid scheduling algorithm.\n");
    }

    return 0;
}
