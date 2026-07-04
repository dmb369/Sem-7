import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results.csv")

plt.figure(figsize=(8,6))
for bsize in df["BlockSize"].unique():
    data = df[df["BlockSize"] == bsize]
    plt.plot(data["Threads"], data["Speedup"], marker="o", label=f"Block {bsize}")

plt.xlabel("Number of Threads")
plt.ylabel("Speedup")
plt.title("Speedup vs Threads (Blocked Matrix Multiplication)")
plt.legend()
plt.grid(True)
plt.savefig("speedup_vs_threads.png", dpi=300)

plt.figure(figsize=(8,6))
for bsize in df["BlockSize"].unique():
    data = df[df["BlockSize"] == bsize]
    plt.plot(data["Threads"], data["AverageTime(s)"], marker="o", label=f"Block {bsize}")

plt.xlabel("Number of Threads")
plt.ylabel("Average Runtime (s)")
plt.title("Runtime vs Threads (Blocked Matrix Multiplication)")
plt.legend()
plt.grid(True)
plt.savefig("runtime_vs_threads.png", dpi=300)


plt.show()
