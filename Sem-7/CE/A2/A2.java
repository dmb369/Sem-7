// File: A2.java
// Purpose: Randomness tests (Chi-Square and KS) using Java Random

import java.util.*;

public class A2 
{
    static int N_CHI = 5000;
    static int RANGE = 1000;
    static int N_KS = 30;
    static int INTERVALS = 10;

    static int[] seeds = {10,20,30,40,50,60,70,80,90,100,250,500,900};

    static double chiSquareTest(int seed) 
    {
        Random rand = new Random(seed);
        int[] freq = new int[INTERVALS];

        for (int i=0; i<N_CHI; i++)
        {
            int num = rand.nextInt(RANGE) + 1;
            int idx = (num-1)/(RANGE/INTERVALS);
            freq[idx]++;
        }

        double expected = (double)N_CHI/INTERVALS;
        double chi = 0.0;

        for (int f : freq) 
        {
            chi += Math.pow(f - expected, 2)/expected;
        }
        return chi;
    }

    static double ksTest(int seed) 
    {
        Random rand = new Random(seed);
        double[] arr = new double[N_KS];

        for (int i=0; i<N_KS; i++) 
        {
            arr[i] = rand.nextDouble()*5.0;
        }

        Arrays.sort(arr);
        double d = 0.0;

        for (int i=0; i<N_KS; i++) 
        {
            double yi = arr[i];
            double F = yi/5.0;
            double Fi = (double)(i+1)/N_KS;
            double diff1 = Math.abs(Fi - F);
            double diff2 = Math.abs(F - (double)i/N_KS);
            d = Math.max(d, Math.max(diff1, diff2));
        }
        return d;
    }

    public static void main(String[] args) 
    {
        double chi95 = 16.92, chi99 = 21.67;
        double ks95 = 0.242, ks99 = 0.298;

        System.out.println("Chi-Square Test (df=9)");
        for (int s : seeds) 
        {
            double chi = chiSquareTest(s);
            System.out.printf("Seed %d -> Chi2 = %.3f -> %s (95%%) | %s (99%%)%n", 
                s, chi,
                (chi < chi95 ? "Pass" : "Fail"),
                (chi < chi99 ? "Pass" : "Fail"));
        }

        System.out.println("\nKS Test (n=30)");
        for (int s : seeds) 
        {
            double ks = ksTest(s);
            System.out.printf("Seed %d -> KS = %.3f -> %s (95%%) | %s (99%%)%n",
                s, ks,
                (ks < ks95 ? "Pass" : "Fail"),
                (ks < ks99 ? "Pass" : "Fail"));
        }
    }
}
