// File: minmaxmean_chunked.c
// Purpose: Parallel Min, Max, and Mean using OpenMP with on-the-fly chunking
// Usage:   gcc -O3 -fopenmp minmaxmean_chunked.c -o minmaxmean_chunked
// Example: ./minmaxmean_chunked 34 > results.csv

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <omp.h>
#include <limits.h>

static inline double timeval_to_sec(const struct timeval *t)
{
    return t->tv_sec + t->tv_usec * 1e-6;
}

// xorshift RNG
static inline uint32_t xorshift32(uint32_t *state)
{
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

int main(int argc, char **argv)
{
    int log2N = 26; // default ~67M
    if(argc >= 2) log2N = atoi(argv[1]);
    uint64_t N = 1ULL << log2N;

    const uint32_t DOMAIN_MAX = 1000000000u;
    const int RUNS = 5;
    const uint64_t CHUNK_SIZE = 100000000ULL; // 100 million

    int thread_list[] = {1,2,4,6,8,10,12,14,16};
    int tcount = sizeof(thread_list)/sizeof(thread_list[0]);

    printf("# N = %llu elements (2^%d)\n", (unsigned long long)N, log2N);
    printf("threads,avg_time_sec,speedup\n");

    double baseline_time = -1.0;

    for(int idx=0; idx<tcount; idx++)
    {
        int threads = thread_list[idx];
        omp_set_num_threads(threads);

        double total_time = 0.0;

        for(int run=0; run<RUNS; run++)
        {
            struct timeval t0, t1;
            gettimeofday(&t0, NULL);

            uint32_t global_min = UINT32_MAX;
            uint32_t global_max = 0;
            long double global_sum = 0.0L;

            // Process in chunks
            for(uint64_t start=0; start<N; start += CHUNK_SIZE)
            {
                uint64_t end = start + CHUNK_SIZE;
                if(end > N) end = N;

                #pragma omp parallel reduction(min:global_min) reduction(max:global_max) reduction(+:global_sum)
                {
                    int tid = omp_get_thread_num();
                    uint32_t rng_state = 123456789u + tid*1337u + run*17u + start;
                    #pragma omp for schedule(static)
                    for(uint64_t i=start; i<end; i++)
                    {
                        uint32_t rnum = xorshift32(&rng_state);
                        uint64_t v = (uint64_t)rnum * (DOMAIN_MAX+1ULL) / (uint64_t)UINT32_MAX;
                        if(v > DOMAIN_MAX) v = DOMAIN_MAX;
                        uint32_t val = (uint32_t)v;

                        if(val < global_min) global_min = val;
                        if(val > global_max) global_max = val;
                        global_sum += val;
                    }
                } // omp parallel
            } // chunk loop

            // Edited
            long double global_mean = global_sum / (long double)N;
            // printf("min=%u max=%u mean=%Lf\n", gmin, gmax, mean);

            gettimeofday(&t1, NULL);
            double elapsed = timeval_to_sec(&t1) - timeval_to_sec(&t0);
            total_time += elapsed;
        } // run loop

        double avg_time = total_time / RUNS;
        if(threads == 1) baseline_time = avg_time;
        double speedup = baseline_time / avg_time;

        printf("%d,%.6f,%.6f\n", threads, avg_time, speedup);
        fflush(stdout);
    } // threads loop

    return 0;
}
