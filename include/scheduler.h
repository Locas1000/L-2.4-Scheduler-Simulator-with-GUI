#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdlib.h>
#include <stdio.h>

#define MAX_PROCESSES 100 // Maximum number of processes

// Represents a single process in the simulator
typedef struct {
    int pid;                // Process ID
    int arrival_time;       // When process arrives
    int burst_time;         // Total CPU time needed
    int priority;           // Priority (lower = higher priority)
    int remaining_time;     // Time left to execute
    int start_time;         // First time scheduled
    int completion_time;    // When finished
    int turnaround_time;    // completion - arrival
    int waiting_time;       // turnaround - burst
    int response_time;      // start - arrival
} process_t;

// Represents a slice of execution on the Gantt chart
typedef struct {
    int time;               // Time slice start
    int pid;                // Process running
    int duration;           // How long it ran
} timeline_event_t;

// --- Part 3: Metrics ---
typedef struct {
    double avg_turnaround_time;
    double avg_waiting_time;
    double avg_response_time;
    double cpu_utilization;
    double throughput;
    double fairness_index;      // Jain's fairness index
} metrics_t;

// --- Function Prototypes ---

// Algorithm 1: FIFO
void schedule_fifo(process_t *processes, int n, timeline_event_t *timeline);

// Algorithm 2: SJF
void schedule_sjf(process_t *processes, int n, timeline_event_t *timeline);

// Algorithm 3: STCF
void schedule_stcf(process_t *processes, int n, timeline_event_t *timeline);

// Algorithm 4: Round Robin
void schedule_rr(process_t *processes, int n, int quantum, timeline_event_t *timeline);

// Algorithm 5: MLFQ
typedef struct {
    int num_queues;
    int *quantums;      // Array of quantums for each queue
    int boost_interval; // Priority boost interval
} mlfq_config_t;

void schedule_mlfq(process_t *processes, int n, mlfq_config_t *config, timeline_event_t *timeline);

// Metrics Calculation
void calculate_metrics(process_t *processes, int n, int total_time, metrics_t *metrics);

#endif // SCHEDULER_H