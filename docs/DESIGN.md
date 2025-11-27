# Design Document

## Data Structures

### Process Control Block (process_t)
We included a `remaining_time` field in the `process_t` struct specifically to support preemptive algorithms like **STCF (Shortest Time to Completion First)** and **Round Robin**.

Unlike non-preemptive algorithms (FIFO, SJF) where a process runs to completion once started, preemptive algorithms can pause a process mid-execution. `remaining_time` allows the scheduler to track exactly how much work is left for a paused process, whereas `burst_time` remains static to preserve the original job length for metric calculations.

### Round Robin Implementation
We implemented Round Robin using a circular queue. A critical design choice was the **order of re-queuing**: when a process finishes its quantum, we first check for *newly arrived* processes and add them to the queue *before* adding the current process back. This ensures better fairness for new arrivals."