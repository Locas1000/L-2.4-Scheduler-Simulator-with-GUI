#include "scheduler.h"

void calculate_metrics(process_t *processes, int n, int total_time, metrics_t *metrics) {
    double total_turnaround = 0;
    double total_waiting = 0;
    double total_response = 0;
    double total_burst = 0;
    double sum_sq_turnaround = 0; // For fairness calculation

    for (int i = 0; i < n; i++) {
        total_turnaround += processes[i].turnaround_time;
        total_waiting += processes[i].waiting_time;
        total_response += processes[i].response_time;
        total_burst += processes[i].burst_time;

        // Sum of squares for Jain's Fairness Index
        sum_sq_turnaround += (processes[i].turnaround_time * processes[i].turnaround_time);
    }

    // Averages
    metrics->avg_turnaround_time = total_turnaround / n;
    metrics->avg_waiting_time = total_waiting / n;
    metrics->avg_response_time = total_response / n;

    // CPU Utilization = (Busy Time / Total Simulation Time) * 100
    if (total_time > 0) {
        metrics->cpu_utilization = (total_burst / total_time) * 100.0;
        metrics->throughput = (double)n / total_time;
    } else {
        metrics->cpu_utilization = 0;
        metrics->throughput = 0;
    }

    // Jain's Fairness Index = (Sum(x))^2 / (n * Sum(x^2))
    // We use Turnaround Time as the metric 'x' for fairness
    if (sum_sq_turnaround > 0) {
        metrics->fairness_index = (total_turnaround * total_turnaround) / (n * sum_sq_turnaround);
    } else {
        metrics->fairness_index = 0;
    }
}