import pandas as pd
import matplotlib.pyplot as plt
import os

def plot_speedup(csv_path, out_path):
    """
    Plot the speedup curve:  speedup = T1 / Tn
    where T1 is the average runtime at 1 thread.
    """
    df = pd.read_csv(csv_path)  # expects columns: threads,time_s
    if 'threads' not in df.columns or 'time_s' not in df.columns:
        print(f"CSV {csv_path} does not contain 'threads,time_s' columns.")
        return

    # compute speedup relative to single-thread time
    t1 = float(df[df['threads'] == 1]['time_s'])
    df['speedup'] = t1 / df['time_s']

    # plot speedup vs thread count
    plt.figure(figsize=(7,5))
    plt.plot(df['threads'], df['speedup'], marker='o', color='forestgreen', label='Measured speedup')
    plt.plot(df['threads'], df['threads'], 'k--', label='Ideal linear speedup')  # optional reference line
    plt.title(f"Speedup Curve: {os.path.basename(csv_path)}")
    plt.xlabel("Number of Threads")
    plt.ylabel("Speedup (T1 / Tn)")
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    plt.close()
    print(f"Saved speedup plot to {out_path}")

if __name__ == "__main__":
    os.makedirs("results", exist_ok=True)
    plot_speedup("results/q5_a_speedup.csv", "results/q5_a_speedup_curve.png")
    plot_speedup("results/q5_b_speedup.csv", "results/q5_b_speedup_curve.png")
