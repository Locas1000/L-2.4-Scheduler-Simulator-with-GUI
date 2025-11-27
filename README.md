### Step 1: The README.md

This is the first thing the TA will look at. It needs to explain how to compile and run your code.

Create a file named `README.md` in the root folder and paste this:

````markdown
# CPU Scheduler Simulator

A graphical simulation of CPU scheduling algorithms, implemented in C using GTK+3.

## 📋 Overview
This project visualizes how different scheduling algorithms manage process execution. It calculates key performance metrics (Turnaround Time, Waiting Time, Response Time) to compare algorithm efficiency.

**Supported Algorithms:**
1. **FIFO** (First-In, First-Out)
2. **SJF** (Shortest Job First)
3. **STCF** (Shortest Time to Completion First)
4. **Round Robin** (Time Quantum = 3)
5. **MLFQ** (Multi-Level Feedback Queue)

## 🔧 Prerequisites
- GCC Compiler
- Make
- GTK+3 Development Libraries

**Installation on Linux (Debian/Ubuntu/Mint):**
```bash
sudo apt-get install build-essential libgtk-3-dev
````

## 🚀 How to Run

1.  **Compile the project:**

    ```bash
    make
    ```

2.  **Run the GUI Simulator:**

    ```bash
    ./scheduler_gui
    ```

3.  **Usage:**

    - The default workload is loaded automatically.
    - Click **"Add Process"** to define custom jobs.
    - Select an algorithm from the dropdown.
    - Click **"Run Simulation"** to view the Gantt chart and metrics.

## 📂 Project Structure

- `src/`: Source code (algorithms, metrics, GUI).
- `include/`: Header files.
- `obj/`: Object files (created during build).
- `docs/`: Design and analysis documentation.

## 📊 Metrics Explained

- **Turnaround Time:** Completion Time - Arrival Time
- **Waiting Time:** Turnaround Time - Burst Time
- **Response Time:** Start Time - Arrival Time

<!-- end list -->

````

---

### Step 2: The Design Document (DESIGN.md)
We started this earlier. Let's finish it. This proves you understand *how* your code works.

Update `docs/DESIGN.md` with this complete content:

```markdown
# Design Document

## 1. Data Structures

### Process Control Block (`process_t`)
We used a struct to represent processes. Key design choices:
- **`remaining_time`**: Added specifically for preemptive algorithms (STCF, RR, MLFQ). It allows us to pause a process and resume it later without losing track of progress.
- **`start_time`**: initialized to `-1`. This helps us detect exactly when a process gets the CPU for the *very first time*, which is critical for calculating **Response Time**.

## 2. Algorithm Implementation Details

### Round Robin
We implemented RR using a standard queue logic.
- **Fairness Strategy:** When a process finishes its quantum, we check for *new arrivals* before putting the current process back in the queue. This prevents new jobs from waiting excessively behind a long-running cycle of existing jobs.

### MLFQ (Multi-Level Feedback Queue)
- **Structure:** 3 Priority Queues.
- **Aging/Boost:** To prevent starvation, a global timer tracks how long since the last boost. Every 10 time units, *all* processes are reset to Priority 0.
- **Demotion:** If a process uses its full time slice at a high priority, it is punished by moving it down a queue.

## 3. GUI Architecture
- **Library:** GTK+3
- **Rendering:** We used the **Cairo** graphics library to draw the Gantt chart dynamically. The chart scales horizontally based on the total simulation time to fit the window.
- **Data Flow:** The GUI reads directly from a `GtkListStore`, converts the data to our `process_t` array, runs the simulation, and then repaints the drawing area.
````

-----

### Step 3: The Performance Analysis

Part 5 of your assignment asks for a report. You can create this as `docs/ANALYSIS.pdf` (or just a text file).

**Quick guide to generating this data:**

1.  Open your GUI.
2.  Run the default workload (P1, P2, P3) with **FIFO**. Write down the "Avg Turnaround Time".
3.  Switch to **SJF**, run it, write down the number.
4.  Switch to **STCF**, run it, write down the number.

**Expected Analysis (Cheat Sheet):**

* **FIFO** will likely be the worst because P1 is long (Convoy Effect).
* **STCF** should be the best (lowest turnaround).
* **RR** will have great Response Time but worse Turnaround Time.

You can summarize this in a simple table inside a text file named `docs/REPORT.txt`.

-----

### Step 4: Final Submission

You are ready to zip it up\!

1.  **Clean your build:**

    ```bash
    make clean
    ```

    *(This removes the `obj` folder and the executable so the zip file isn't huge)*.

2.  **Zip the folder:**

    ```bash
    cd ..
    zip -r homework3_submission.zip homework3/
    ```
