import numpy as np, matplotlib.pyplot as plt

def plot_one(path, title):
    x = np.fromfile(path, dtype=np.uint64)
    if x.size == 0: print("No data:", path); return
    plt.figure()
    plt.boxplot(x, vert=True, showfliers=False)
    plt.title(title); plt.tight_layout()
    out = path.replace('.bin','_box.png')
    plt.savefig(out, dpi=150); print("Saved", out)

plot_one('results/sample_a.bin', 'Q5 (a) Box Plot')
plot_one('results/sample_b.bin', 'Q5 (b) Box Plot')
