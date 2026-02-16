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
```

## 🚀 How to Run

1. **Compile the project:**
   ```bash
   make
   ```

2. **Run the GUI Simulator:**
   ```bash
   ./scheduler_gui
   ```

3. **Usage:**
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

## 🧹 Cleaning Build Files

To remove compiled files:
```bash
make clean
```

## 📦 Preparing Submission

1. Clean your build:
   ```bash
   make clean
   ```

2. Zip the folder:
   ```bash
   cd ..
   zip -r homework3_submission.zip L-2.4-Scheduler-Simulator-with-GUI/
   ```

## 📖 Documentation

- See `docs/DESIGN.md` for implementation details
- See `docs/report.txt` for performance analysis
