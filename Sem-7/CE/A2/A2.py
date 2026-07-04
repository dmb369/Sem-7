# File: A2.py
# Purpose: Randomness tests (Chi-Square and KS) using Python's random

import random
import math

N_CHI = 5000
RANGE = 1000
N_KS = 30
INTERVALS = 10   # for Chi-Square

seeds = [10,20,30,40,50,60,70,80,90,100,250,500,900]

def chi_square_test(seed):
    random.seed(seed)

    freq = [0]*INTERVALS
    for _ in range(N_CHI):
        num = random.randint(1, RANGE)
        idx = (num-1)//(RANGE//INTERVALS)
        freq[idx] += 1

    expected = N_CHI/INTERVALS
    chi = sum(((f-expected)**2)/expected for f in freq)

    return chi

def ks_test(seed):
    random.seed(seed)

    arr = [random.uniform(0,5) for _ in range(N_KS)]
    arr.sort()

    d = 0.0
    for i, yi in enumerate(arr, start=1):
        F = yi/5.0
        Fi = i/N_KS
        diff1 = abs(Fi - F)
        diff2 = abs(F - (i-1)/N_KS)
        d = max(d, diff1, diff2)

    return d

chi95, chi99 = 16.92, 21.67
ks95, ks99 = 0.242, 0.298

print("Chi-Square Test (df=9)")
for s in seeds:
    chi = chi_square_test(s)
    print(f"Seed {s} -> Chi2 = {chi:.3f} -> {'Pass' if chi < chi95 else 'Fail'} (95%) | {'Pass' if chi < chi99 else 'Fail'} (99%)")

print("\nKS Test (n=30)")
for s in seeds:
    ks = ks_test(s)
    print(f"Seed {s} -> KS = {ks:.3f} -> {'Pass' if ks < ks95 else 'Fail'} (95%) | {'Pass' if ks < ks99 else 'Fail'} (99%)")
