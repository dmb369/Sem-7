# File: plot_speedup.py
# Purpose: Plot speedup and runtime from results.csv
import pandas as pd
import matplotlib.pyplot as plt

# Load CSV (skip comment line)
df = pd.read_csv("results_2.csv", comment="#")

# Plot speedup
plt.figure(figsize=(7,5))
plt.plot(df["threads"], df["speedup"], marker="o", label="Measured speedup")
plt.plot(df["threads"], df["threads"], linestyle="--", color="gray", label="Ideal speedup")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.title("Speedup vs Threads")
plt.legend()
plt.grid(True)
plt.savefig("speedup.png", dpi=150)
plt.show()

# Plot runtime
plt.figure(figsize=(7,5))
plt.plot(df["threads"], df["avg_time_sec"], marker="s", color="red")
plt.xlabel("Threads")
plt.ylabel("Average Runtime (sec)")
plt.title("Runtime vs Threads")
plt.grid(True)
plt.savefig("runtime.png", dpi=150)
plt.show()
