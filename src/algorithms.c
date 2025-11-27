#include <stdio.h>
#include <stdbool.h>
#include "scheduler.h"

// ------------------------------------------------------
// Algorithm 1: FIFO (First In First Out)
// ------------------------------------------------------
void schedule_fifo(process_t *processes, int n, timeline_event_t *timeline) {
    int current_time = 0;

    for (int i = 0; i < n; i++) {
        process_t *p = &processes[i];

        if (p->arrival_time > current_time) {
            current_time = p->arrival_time;
        }

        p->start_time = current_time;
        p->completion_time = p->start_time + p->burst_time;

        p->turnaround_time = p->completion_time - p->arrival_time;
        p->waiting_time = p->turnaround_time - p->burst_time;
        p->response_time = p->start_time - p->arrival_time;

        timeline[i].time = p->start_time;
        timeline[i].pid = p->pid;
        timeline[i].duration = p->burst_time;

        current_time += p->burst_time;
    }
}

// ------------------------------------------------------
// Algorithm 2: SJF (Shortest Job First)
// ------------------------------------------------------
void schedule_sjf(process_t *processes, int n, timeline_event_t *timeline) {
    int current_time = 0;
    int completed = 0;
    int is_completed[MAX_PROCESSES] = {0};
    int timeline_index = 0;

    while (completed < n) {
        int shortest_index = -1;
        int min_burst = 999999;

        // Find shortest arrived job
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && !is_completed[i]) {
                if (processes[i].burst_time < min_burst) {
                    min_burst = processes[i].burst_time;
                    shortest_index = i;
                }
                else if (processes[i].burst_time == min_burst) {
                    if (processes[i].arrival_time < processes[shortest_index].arrival_time) {
                        shortest_index = i;
                    }
                }
            }
        }

        if (shortest_index == -1) {
            current_time++;
        }
        else {
            process_t *p = &processes[shortest_index];

            p->start_time = current_time;
            p->completion_time = p->start_time + p->burst_time;

            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;
            p->response_time = p->start_time - p->arrival_time;

            timeline[timeline_index].time = p->start_time;
            timeline[timeline_index].pid = p->pid;
            timeline[timeline_index].duration = p->burst_time;
            timeline_index++;

            is_completed[shortest_index] = 1;
            completed++;
            current_time += p->burst_time;
        }
    }
}

// ------------------------------------------------------
// Algorithm 3: STCF (Shortest Time to Completion First)
// ------------------------------------------------------
void schedule_stcf(process_t *processes, int n, timeline_event_t *timeline) {
    int current_time = 0;
    int completed = 0;
    int timeline_index = 0;

    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
    }

    while (completed < n) {
        int shortest_index = -1;
        int min_remaining = 999999;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].remaining_time < min_remaining) {
                    min_remaining = processes[i].remaining_time;
                    shortest_index = i;
                }
                else if (processes[i].remaining_time == min_remaining) {
                    if (processes[i].arrival_time < processes[shortest_index].arrival_time) {
                        shortest_index = i;
                    }
                }
            }
        }

        if (shortest_index == -1) {
            current_time++;
        }
        else {
            process_t *p = &processes[shortest_index];

            if (p->remaining_time == p->burst_time) {
                p->start_time = current_time;
            }

            timeline[timeline_index].time = current_time;
            timeline[timeline_index].pid = p->pid;
            timeline[timeline_index].duration = 1;
            timeline_index++;

            p->remaining_time--;
            current_time++;

            if (p->remaining_time == 0) {
                completed++;
                p->completion_time = current_time;
                p->turnaround_time = p->completion_time - p->arrival_time;
                p->waiting_time = p->turnaround_time - p->burst_time;
                p->response_time = p->start_time - p->arrival_time;
            }
        }
    }
}

// ------------------------------------------------------
// Algorithm 4: Round Robin
// ------------------------------------------------------
void schedule_rr(process_t *processes, int n, int quantum, timeline_event_t *timeline) {
    int current_time = 0;
    int completed = 0;
    int timeline_index = 0;

    int queue[MAX_PROCESSES * 10]; // Larger buffer for safety
    int front = 0;
    int rear = 0;

    int visited[MAX_PROCESSES] = {0};

    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
    }

    // Initial fill
    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time == 0) {
            queue[rear++] = i;
            visited[i] = 1;
        }
    }

    while (completed < n) {
        if (front == rear) {
            int min_arrival = 999999;
            for(int i=0; i<n; i++) {
                if(!visited[i] && processes[i].arrival_time < min_arrival) {
                    min_arrival = processes[i].arrival_time;
                }
            }
            if (min_arrival < 999999) current_time = min_arrival;
            else current_time++;

            for (int i = 0; i < n; i++) {
                if (processes[i].arrival_time <= current_time && !visited[i]) {
                    queue[rear++] = i;
                    visited[i] = 1;
                }
            }
            continue;
        }

        int idx = queue[front++];
        process_t *p = &processes[idx];

        if (p->remaining_time == p->burst_time) {
            p->start_time = current_time;
        }

        int run_time = (p->remaining_time > quantum) ? quantum : p->remaining_time;

        timeline[timeline_index].time = current_time;
        timeline[timeline_index].pid = p->pid;
        timeline[timeline_index].duration = run_time;
        timeline_index++;

        current_time += run_time;
        p->remaining_time -= run_time;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && !visited[i]) {
                queue[rear++] = i;
                visited[i] = 1;
            }
        }

        if (p->remaining_time > 0) {
            queue[rear++] = idx;
        } else {
            completed++;
            p->completion_time = current_time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;
            p->response_time = p->start_time - p->arrival_time;
        }
    }
}

// ------------------------------------------------------
// Algorithm 5: MLFQ (Multi-Level Feedback Queue)
// ------------------------------------------------------
void schedule_mlfq(process_t *processes, int n, mlfq_config_t *config, timeline_event_t *timeline) {
    int current_time = 0;
    int completed = 0;
    int timeline_index = 0;

    // Track how much quantum used at current level
    int time_slice_used[MAX_PROCESSES] = {0};

    // Initialize processes
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].priority = 0; // Start at highest priority (0)
        time_slice_used[i] = 0;
    }

    int time_since_boost = 0;

    while (completed < n) {
        // 1. Check for Priority Boost
        if (time_since_boost >= config->boost_interval) {
            for (int i = 0; i < n; i++) {
                if (processes[i].remaining_time > 0) {
                    processes[i].priority = 0;
                    time_slice_used[i] = 0;
                }
            }
            time_since_boost = 0;
        }

        // 2. Find process to run: Highest Priority (lowest value) that has arrived
        int selected_idx = -1;
        int highest_prio = 999;

        // Iterate through priority queues (0 to num_queues-1)
        for (int q = 0; q < config->num_queues; q++) {
            bool found_in_queue = false;

            // Check all processes to see if any are in this queue and ready
            // (Note: In a real OS, we'd have actual queues. Here we scan.)
            for (int i = 0; i < n; i++) {
                if (processes[i].arrival_time <= current_time &&
                    processes[i].remaining_time > 0 &&
                    processes[i].priority == q) {

                    // Found a candidate.
                    // To strictly follow Round Robin within queue, we should track "last run".
                    // For this sim, we pick the first one found (FCFS within priority)
                    // or implement a simple pointer rotation if needed.
                    selected_idx = i;
                    highest_prio = q;
                    found_in_queue = true;
                    break;
                }
            }
            if (found_in_queue) break;
        }

        // 3. Run Logic
        if (selected_idx == -1) {
            current_time++;
            time_since_boost++;
        }
        else {
            process_t *p = &processes[selected_idx];

            if (p->remaining_time == p->burst_time) {
                p->start_time = current_time;
            }

            // Run for 1 tick
            timeline[timeline_index].time = current_time;
            timeline[timeline_index].pid = p->pid;
            timeline[timeline_index].duration = 1;
            timeline_index++;

            p->remaining_time--;
            time_slice_used[selected_idx]++;
            current_time++;
            time_since_boost++;

            // Check completion
            if (p->remaining_time == 0) {
                completed++;
                p->completion_time = current_time;
                p->turnaround_time = p->completion_time - p->arrival_time;
                p->waiting_time = p->turnaround_time - p->burst_time;
                p->response_time = p->start_time - p->arrival_time;
            }
            else {
                // Check if quantum exceeded for this level
                int current_quantum = config->quantums[p->priority];
                if (time_slice_used[selected_idx] >= current_quantum) {
                    // Downgrade priority if not already at bottom
                    if (p->priority < config->num_queues - 1) {
                        p->priority++;
                    }
                    // Reset slice usage for new level
                    time_slice_used[selected_idx] = 0;
                }
            }
        }
    }
}
