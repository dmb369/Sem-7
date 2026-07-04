import numpy as np
import matplotlib.pyplot as plt
import os

def plot_cdf(bin_path, out_path):
    """
    Compute and plot the empirical CDF of a sampled dataset.
    """
    # read the binary sample file as uint64 array
    data = np.fromfile(bin_path, dtype=np.uint64)
    if data.size == 0:
        print(f"No data found in {bin_path}")
        return

    # sort the data
    data_sorted = np.sort(data)
    # compute cumulative probabilities: 1/N, 2/N, ..., N/N
    cdf = np.arange(1, data_sorted.size + 1) / data_sorted.size

    plt.figure(figsize=(8,5))
    plt.plot(data_sorted, cdf, color='darkorange', lw=2)
    plt.title(f"CDF of {os.path.basename(bin_path).split('.')[0]}")
    plt.xlabel("Value")
    plt.ylabel("Cumulative Probability")
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    plt.close()
    print(f"Saved CDF plot to {out_path}")

if __name__ == "__main__":
    os.makedirs("results", exist_ok=True)
    # Create CDF plots for both datasets
    plot_cdf("results/sample_a.bin", "results/q5_a_cdf.png")
    plot_cdf("results/sample_b.bin", "results/q5_b_cdf.png")
