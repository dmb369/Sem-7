import pandas as pd
import matplotlib.pyplot as plt, sys
path = sys.argv[1]
df = pd.read_csv(path)
t1 = float(df[df.threads==1].time_s)
df['speedup'] = t1/df.time_s
plt.figure()
plt.plot(df.threads, df.speedup, marker='o')
plt.xlabel('Threads'); plt.ylabel('Speedup')
plt.title(('Q5 (a)' if '_a_' in path else 'Q5 (b)')+' Speedup')
plt.grid(True); plt.tight_layout()
out = path.replace('.csv','_speedup.png')
plt.savefig(out, dpi=150); print("Saved", out)
