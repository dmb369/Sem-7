import pandas as pd
import matplotlib.pyplot as plt
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
import os

report_path = "./report.txt"

# === Read report ===
with open(report_path, "r") as f:
    lines = f.readlines()

rows = []
for line in lines:
    if "|TCP" in line or "|UDP" in line:
        parts = line.split()
        # Expected structure: key src dst protocol total_packets total_bytes duration pkt_rate syn unique_dst_ports anomaly_score
        if len(parts) >= 11:
            key = parts[0]
            protocol = parts[3]
            try:
                total_packets = int(parts[4])
                total_bytes = int(parts[5])
                duration = float(parts[6])
                pkt_rate = float(parts[7])
                syn = int(parts[8])
                unique_dst_ports = int(parts[9])
                anomaly_score = float(parts[10])
                rows.append([
                    key, protocol, total_packets, total_bytes, duration,
                    pkt_rate, syn, unique_dst_ports, anomaly_score
                ])
            except ValueError:
                continue

if not rows:
    raise ValueError("⚠️ Could not parse flow data — check that your table in report.txt matches the example with TCP/UDP entries.")

# === Create DataFrame ===
cols = ["key", "protocol", "total_packets", "total_bytes", "duration",
        "pkt_rate", "syn", "unique_dst_ports", "anomaly_score"]
df = pd.DataFrame(rows, columns=cols)

# === Create plots folder ===
os.makedirs("plots", exist_ok=True)

# === Plot 1: Anomaly scores ===
df_sorted = df.sort_values("anomaly_score", ascending=True).reset_index(drop=True)

plt.figure(figsize=(8, 5))
plt.plot(df_sorted.index, df_sorted["anomaly_score"], linewidth=1.5)
plt.xlabel("Flow index (sorted)")
plt.ylabel("Anomaly score (IsolationForest)")
plt.title("Anomaly Scores (lower → more anomalous)")
plt.grid(True)
plt.tight_layout()
plt.savefig("plots/anomaly_scores_from_txt.png", dpi=300)
plt.show()

# === Plot 2: PCA visualization ===
numeric_df = df[["total_packets", "total_bytes", "duration", "pkt_rate", "syn", "unique_dst_ports"]]

scaler = StandardScaler()
scaled = scaler.fit_transform(numeric_df)

pca = PCA(n_components=2)
pca_res = pca.fit_transform(scaled)

df_pca = pd.DataFrame(pca_res, columns=["PC1", "PC2"])
df_pca["anomaly_score"] = df["anomaly_score"]

threshold = df["anomaly_score"].quantile(0.1)
df_pca["is_anomaly"] = df_pca["anomaly_score"] <= threshold

plt.figure(figsize=(7, 6))
plt.scatter(df_pca[~df_pca["is_anomaly"]]["PC1"], df_pca[~df_pca["is_anomaly"]]["PC2"],
            c="blue", s=10, label="Normal")
plt.scatter(df_pca[df_pca["is_anomaly"]]["PC1"], df_pca[df_pca["is_anomaly"]]["PC2"],
            c="red", s=15, label="Anomaly")
plt.xlabel("PCA Component 1")
plt.ylabel("PCA Component 2")
plt.title("PCA of Flow Features (red = anomaly)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("plots/pca_from_txt.png", dpi=300)
plt.show()

print("\n✅ Plots saved in 'plots/' folder:")
print(" - anomaly_scores_from_txt.png")
print(" - pca_from_txt.png")
