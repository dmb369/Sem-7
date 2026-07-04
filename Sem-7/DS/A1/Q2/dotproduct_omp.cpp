// File: dotproduct_omp.c
// Purpose: Parallel dot product of two vectors using OpenMP with on-the-fly generation
// Usage:   gcc -O3 -fopenmp dotproduct_omp.c -o dotproduct_omp
// Example: ./dotproduct_omp > results.csv

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

static inline double timeval_to_sec(const struct timeval *t)
{
    return t->tv_sec + t->tv_usec * 1e-6;
}

// Simple xorshift RNG
static inline uint32_t xorshift32(uint32_t *state)
{
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

// Generate random number from {-1, 0, 1} based on RNG
static inline int rand_ternary(uint32_t *state)
{
    return (int)(xorshift32(state) % 3) - 1;  // maps 0->-1, 1->0, 2->1
}

int main(int argc, char **argv)
{
    uint64_t N = 1000000000ULL; // 10^9 elements
    const int RUNS = 5;
    const uint64_t CHUNK_SIZE = 100000000ULL; // 100M per chunk

    int thread_list[] = {1,2,4,6,8,10,12,14,16};
    int tcount = sizeof(thread_list)/sizeof(thread_list[0]);

    printf("# N = %llu elements\n", (unsigned long long)N);
    printf("threads,avg_time_sec,speedup,dot_product\n");

    double baseline_time = -1.0;

    for(int idx=0; idx<tcount; idx++)
    {
        int threads = thread_list[idx];
        omp_set_num_threads(threads);

        double total_time = 0.0;
        long long final_dot = 0; // store final dot product

        for(int run=0; run<RUNS; run++)
        {
            struct timeval t0, t1;
            gettimeofday(&t0, NULL);

            long long dot_product = 0;

            for(uint64_t start=0; start<N; start += CHUNK_SIZE)
            {
                uint64_t end = start + CHUNK_SIZE;
                if(end > N) end = N;

                #pragma omp parallel reduction(+:dot_product)
                {
                    int tid = omp_get_thread_num();
                    uint32_t rng_state1 = 123456789u + tid*1337u + run*17u + start;
                    uint32_t rng_state2 = 987654321u + tid*31337u + run*17u + start;

                    #pragma omp for schedule(static)
                    for(uint64_t i=start; i<end; i++)
                    {
                        int a = rand_ternary(&rng_state1);
                        int b = rand_ternary(&rng_state2);
                        dot_product += (long long)a * (long long)b;
                    }
                } // omp parallel
            } // chunk loop

            final_dot = dot_product;
            gettimeofday(&t1, NULL);
            double elapsed = timeval_to_sec(&t1) - timeval_to_sec(&t0);
            total_time += elapsed;
        } // run loop

        double avg_time = total_time / RUNS;
        if(threads == 1) baseline_time = avg_time;
        double speedup = baseline_time / avg_time;

        printf("%d,%.6f,%.6f,%lld\n", threads, avg_time, speedup, final_dot);
        fflush(stdout);
    } // threads loop

    return 0;
}
