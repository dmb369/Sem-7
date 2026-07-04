import numpy as np
import matplotlib.pyplot as plt
import os
import sys

def plot_histogram(bin_path, out_path, bins=50):
    """
    Draw a histogram of sampled values from Q5 dataset.
    """
    # read the binary sample file as uint64 array
    data = np.fromfile(bin_path, dtype=np.uint64)
    if data.size == 0:
        print(f"No data found in {bin_path}")
        return

    plt.figure(figsize=(8,5))
    plt.hist(data, bins=bins, color='skyblue', edgecolor='black')
    plt.title(f"Histogram of {os.path.basename(bin_path).split('.')[0]}")
    plt.xlabel("Value")
    plt.ylabel("Frequency")
    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    plt.close()
    print(f"Saved histogram to {out_path}")

if __name__ == "__main__":
    # default: generate histograms for both Q5(a) and Q5(b)
    os.makedirs("results", exist_ok=True)
    plot_histogram("results/sample_a.bin", "results/q5_a_histogram.png")
    plot_histogram("results/sample_b.bin", "results/q5_b_histogram.png")
