Matrix Multiplication with OpenMP
-----------------------------------------

Author: Shaik Naveed Ahmed
Roll Number: SE22UCSE252

Description:
------------
This project implements parallel matrix multiplication using OpenMP.
The goal is to evaluate performance by varying:
    - Matrix size: N = 4096 (default, can override at runtime).
    - Block sizes: 2, 4, 8, 16, 32.
    - Number of threads: 1, 2, 4, 6, 8, 10, 12, 14, 16.

The program measures:
    - Average runtime over 5 runs.
    - Speedup relative to single-thread execution.

Results are written to `results.csv` and plotted as graphs
("Runtime vs Threads" and "Speedup vs Threads").

Files:
------
1. blocked_matrix.c   - C source code with OpenMP
2. results.csv        - Generated performance data
3. graphs.py          - Python script to generate graphs
4. report.pdf         - Report containing analysis and plots

Compilation:
------------
Use GCC with OpenMP support:

    gcc -O3 -fopenmp -o blocked_matrix blocked_matrix.c

Execution:
----------
Default run (N=4096):

    ./blocked_matrix

Outputs:
--------
1. Console output: runtime per run, average runtime, speedup.
2. results.csv: contains block size, threads, average runtime, speedup.
3. Graphs (via Python script):
       python graphs.py

   Produces:
       - Speedup vs Threads
       - Runtime vs Threads

Notes:
------
- Random input matrices are generated from uniform distribution [0,1].
- Result matrix is not compared against sequential output (but approximate correctness checked).
- Larger block sizes exploit cache better; very small blocks add overhead.

How it works:
-------------
- The program generates two random N×N matrices A and B with values in [0,1].
- It multiplies them using **blocked (tiled) matrix multiplication**, which improves cache efficiency.
- OpenMP is used to parallelize the computation across multiple threads.
- The program measures runtime over 5 runs for each combination of block size and thread count.
- Results (average time and speedup) are written to results.csv for analysis.
- Block sizes affect performance: small blocks add overhead, large blocks reduce parallelism.
