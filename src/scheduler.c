#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"

#define MAX_TIMELINE 1000

// Helper to reset process states between algorithms
void reset_processes(process_t *p, int n) {
    // Workload 1 Hardcoded values for reset
    // PID, Arrival, Burst, Priority
    int data[3][4] = {
        {1, 0, 5, 1},
        {2, 1, 3, 2},
        {3, 2, 8, 1}
    };

    for(int i=0; i<n; i++) {
        p[i].pid = data[i][0];
        p[i].arrival_time = data[i][1];
        p[i].burst_time = data[i][2];
        p[i].priority = data[i][3];
        p[i].remaining_time = p[i].burst_time;
        p[i].start_time = 0;
        p[i].completion_time = 0;
        p[i].turnaround_time = 0;
        p[i].waiting_time = 0;
        p[i].response_time = 0;
    }
}

void print_metrics(const char *algo_name, metrics_t *m) {
    printf("\n=== %s Results ===\n", algo_name);
    printf("Avg Turnaround: %.2f\n", m->avg_turnaround_time);
    printf("Avg Waiting:    %.2f\n", m->avg_waiting_time);
    printf("Avg Response:   %.2f\n", m->avg_response_time);
    printf("Throughput:     %.2f\n", m->throughput);
    printf("Fairness Idx:   %.2f\n", m->fairness_index);
}

int main() {
    printf("Starting Scheduler Simulation CLI Test...\n");

    int n = 3;
    process_t processes[MAX_PROCESSES];
    timeline_event_t timeline[MAX_TIMELINE];
    metrics_t metrics;

    // --- 1. FIFO ---
    reset_processes(processes, n);
    schedule_fifo(processes, n, timeline);
    calculate_metrics(processes, n, processes[n-1].completion_time, &metrics);
    print_metrics("FIFO", &metrics);

    // --- 2. SJF ---
    reset_processes(processes, n);
    schedule_sjf(processes, n, timeline);
    int max_time = 0;
    for(int i=0; i<n; i++) if(processes[i].completion_time > max_time) max_time = processes[i].completion_time;
    calculate_metrics(processes, n, max_time, &metrics);
    print_metrics("SJF", &metrics);

    // --- 3. STCF ---
    reset_processes(processes, n);
    schedule_stcf(processes, n, timeline);
    max_time = 0;
    for(int i=0; i<n; i++) if(processes[i].completion_time > max_time) max_time = processes[i].completion_time;
    calculate_metrics(processes, n, max_time, &metrics);
    print_metrics("STCF", &metrics);

    // --- 4. Round Robin (q=3) ---
    reset_processes(processes, n);
    schedule_rr(processes, n, 3, timeline);
    max_time = 0;
    for(int i=0; i<n; i++) if(processes[i].completion_time > max_time) max_time = processes[i].completion_time;
    calculate_metrics(processes, n, max_time, &metrics);
    print_metrics("Round Robin (Q=3)", &metrics);

    // --- 5. MLFQ ---
    reset_processes(processes, n);
    mlfq_config_t config;
    config.num_queues = 3;
    int quantums[] = {2, 4, 8};
    config.quantums = quantums;
    config.boost_interval = 10;
    schedule_mlfq(processes, n, &config, timeline);
    max_time = 0;
    for(int i=0; i<n; i++) if(processes[i].completion_time > max_time) max_time = processes[i].completion_time;
    calculate_metrics(processes, n, max_time, &metrics);
    print_metrics("MLFQ", &metrics);

    return 0;
}