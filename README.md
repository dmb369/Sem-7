# Semester 7 — Distributed Systems & Cryptography Coursework

Two courses' worth of assignments: **DS** (Distributed Systems — OpenMP/MPI
parallel computing) and **CE** (Cryptography & Security).

## Repo structure

```
Sem-7/
├── DS/
│   ├── Array.c                        OpenMP: parallel array access demo
│   ├── HWP.c                          OpenMP: parallel "Hello World"
│   ├── SE22UCSE078_Convolution.c      serial vs. OpenMP-parallel 2D convolution
│   ├── SE22UCSE078_Sorting_MPI.c      distributed sort/reduce with MPI
│   └── A1/                            Assignment 1 (team, split by question)
│       ├── README.md                  team + per-question contributions
│       ├── Q1/  min/max/mean over an array, OpenMP, speedup plots
│       ├── Q2/  parallel dot product, OpenMP, speedup plots
│       ├── Q3/  (see DS-Report for spec), speedup/runtime plots
│       ├── Q4/  blocked (tiled) matrix multiplication, OpenMP
│       └── Q5/  streaming statistics benchmark: histograms, CDFs, box plots,
│                runtime-vs-threads and speedup curves across two sample sets
└── CE/
    ├── A1/   Classical cipher implementation + cryptanalysis
    ├── A2/   Randomness testing (Chi-Square / KS tests) — C, Java, Python
    ├── A4/   Network traffic anomaly detection (flow features + IsolationForest + PCA)
    └── A5/   SHA-1 length-extension attack
```

## DS — Distributed Systems (OpenMP / MPI)

Root-level files are short standalone OpenMP/MPI demos (parallel array
access, "hello world", convolution, MPI-based distributed sort).

**A1** is a team assignment split by question, each exploring a different
parallel-performance question via OpenMP, benchmarked across thread counts
with runtime/speedup plots and a written report:

| Question | Topic |
|---|---|
| Q1 | Parallel min/max/mean reduction over an array |
| Q2 | Parallel dot product |
| Q3 | (see `Q3/DS-Report-SE22UCSE062.pdf` for the exact problem) |
| Q4 | Blocked (tiled) matrix multiplication — cache-efficiency vs. thread count |
| Q5 | Streaming statistics on two sample datasets: histograms, CDFs, box plots, runtime-vs-threads and speedup curves |

Compile with OpenMP support, e.g.:
```bash
gcc -O3 -fopenmp -o q1 minmaxmean_omp.cpp
./q1
python3 plot_speedup.py
```
Q4's `Readme.txt` has the fullest worked example of the compile/run/plot
cycle used across the other questions.

Team members and per-question ownership are listed in `DS/A1/README.md`.

## CE — Cryptography & Security

- **A1 — Classical ciphers + cryptanalysis.** `cipher.cpp` implements a
  small cipher class hierarchy (Caesar and others via a common `Cipher`
  interface); `cryptanalysis.cpp` attacks ciphertext using English letter
  frequency statistics and a dictionary (`EnglishWords.txt`).
- **A2 — Randomness testing.** The same Chi-Square and Kolmogorov-Smirnov
  randomness tests are implemented three times — in C (`A2.c`), Java
  (`A2.java`), and Python (`A2.py`) — run across multiple seeds, to compare
  each language's built-in PRNG.
- **A4 — Network traffic anomaly detection.** `plot_analysis.py` reads
  per-flow network traffic features (packet counts, byte counts, duration,
  packet rate, SYN flags, unique destination ports) and applies an
  **IsolationForest** to flag anomalous flows (e.g. port-scan-like
  patterns), with PCA used to visualize the flagged flows. `report.txt`
  contains the full run output and top-40 anomaly table.
- **A5 — SHA-1 length-extension attack.** `message_extension_attack.py`
  demonstrates a length-extension attack against `secret || message`
  SHA-1 MACs using the `hlextend` library, forging a valid hash for an
  attacker-appended message without knowing the secret.

## Notes

- `.DS_Store`, `Thumbs.db`, and the `.vscode/` folder are macOS/editor
  artifacts from the original submission folders — safe to delete or
  `.gitignore` before pushing.
- Reports (`.pdf`, `.docx`) are the original submitted writeups and are
  kept alongside the code they document.
