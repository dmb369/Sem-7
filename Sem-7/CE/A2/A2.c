// File: A2.c
// Purpose: Randomness tests (Chi-Square and KS) on C rand()

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N_CHI 5000
#define RANGE 1000
#define N_KS 30
#define INTERVALS 10   // bins for chi-square

// Predefined 13 seeds
int seeds[13] = {10,20,30,40,50,60,70,80,90,100,250,500,900};

// Chi-square test
double chi_square_test(int seed) 
{
    int freq[INTERVALS] = {0};
    srand(seed);

    for (int i = 0; i < N_CHI; i++) 
    {
        int num = rand() % RANGE + 1;
        int idx = (num - 1) / (RANGE / INTERVALS);
        freq[idx]++;
    }

    double expected = (double)N_CHI / INTERVALS;
    double chi_sq = 0.0;
    for (int i = 0; i < INTERVALS; i++) 
    {
        chi_sq += pow(freq[i] - expected, 2) / expected;
    }

    return chi_sq;
}

// Kolmogorov-Smirnov test
double ks_test(int seed) 
{
    double arr[N_KS];
    srand(seed);

    for (int i = 0; i < N_KS; i++) 
    {
        arr[i] = ((double)rand() / RAND_MAX) * 5.0; // U[0,5]
    }

    // sort
    for (int i = 0; i < N_KS-1; i++) {
        for (int j = i+1; j < N_KS; j++) {
            if (arr[i] > arr[j]) {
                double tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }

    // compute KS statistic
    double d = 0.0;
    for (int i = 0; i < N_KS; i++) 
    {
        double yi = arr[i];
        double F = yi / 5.0;              // theoretical CDF
        double Fi = (double)(i+1) / N_KS; // empirical CDF
        double diff1 = fabs(Fi - F);
        double diff2 = fabs(F - (double)i / N_KS);
        if (diff1 > d) d = diff1;
        if (diff2 > d) d = diff2;
    }
    return d;
}

int main() {
    double chi95 = 16.92, chi99 = 21.67;
    double ks95 = 0.242, ks99 = 0.298;

    printf("Chi-Square Test (df=9)\n");
    for (int i = 0; i < 13; i++) 
    {
        double chi = chi_square_test(seeds[i]);
        printf("Seed %d -> Chi2 = %.3f -> %s (95%%) | %s (99%%)\n", 
               seeds[i], chi,
               (chi < chi95 ? "Pass" : "Fail"),
               (chi < chi99 ? "Pass" : "Fail"));
    }

    printf("\nKS Test (n=30)\n");
    for (int i = 0; i < 13; i++) 
    {
        double ks = ks_test(seeds[i]);
        printf("Seed %d -> KS = %.3f -> %s (95%%) | %s (99%%)\n",
               seeds[i], ks,
               (ks < ks95 ? "Pass" : "Fail"),
               (ks < ks99 ? "Pass" : "Fail"));
    }
    return 0;
}
