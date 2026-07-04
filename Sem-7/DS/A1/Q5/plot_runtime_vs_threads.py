import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

def plot_runtime(csv_path, out_path):
    """
    Plot average runtime (seconds) vs number of threads.
    """
    df = pd.read_csv(csv_path)  # expects columns: threads,time_s
    if 'threads' not in df.columns or 'time_s' not in df.columns:
        print(f"CSV {csv_path} does not contain 'threads,time_s' columns.")
        return

    plt.figure(figsize=(7,5))
    plt.plot(df['threads'], df['time_s'], marker='o', color='steelblue')
    plt.title(f"Runtime vs Threads: {os.path.basename(csv_path)}")
    plt.xlabel("Number of Threads")
    plt.ylabel("Average Runtime (seconds)")
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    plt.close()
    print(f"Saved runtime plot to {out_path}")

if __name__ == "__main__":
    os.makedirs("results", exist_ok=True)
    # create plots for both datasets
    plot_runtime("results/q5_a_speedup.csv", "results/q5_a_runtime_vs_threads.png")
    plot_runtime("results/q5_b_speedup.csv", "results/q5_b_runtime_vs_threads.png")
