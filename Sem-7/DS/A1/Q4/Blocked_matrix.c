#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include <time.h>  

int main(int argc, char *argv[]) {
    int N = 4096; 
    if (argc > 1) N = atoi(argv[1]); 
    int blocks[] = {2,4,8,16,32};
    int nb = sizeof(blocks)/sizeof(blocks[0]);
    int thread_counts[] = {1,2,4,6,8,10,12,14,16};
    int ntc = sizeof(thread_counts)/sizeof(thread_counts[0]);
    const int RUNS = 5;

    printf("Blocked MatMul: N=%d  (use ./a.out <N> to override)\n", N);

    
    FILE *fout = fopen("results.csv", "w");
    if (!fout) {
        fprintf(stderr, "Error: could not open results.csv for writing\n");
        return 1;
    }
    fprintf(fout, "BlockSize,Threads,AverageTime(s),Speedup\n");

    double *A = (double*) malloc((size_t)N * N * sizeof(double));
    double *B = (double*) malloc((size_t)N * N * sizeof(double));
    double *C = (double*) malloc((size_t)N * N * sizeof(double));
    if (!A || !B || !C) { fprintf(stderr,"Allocation failed\n"); return 1; }

    srand(time(NULL)); 
    for (long long i=0;i<(long long)N*(long long)N;i++) {
        A[i] = (double)rand() / RAND_MAX;  
        B[i] = (double)rand() / RAND_MAX;
        C[i] = 0.0;
    }

    for (int bi=0; bi<nb; ++bi) {
        int Bsize = blocks[bi];
        printf("\n--- Block size = %d ---\n", Bsize);

        double baseline_time = 0.0; 

        for (int tci=0; tci<ntc; ++tci) {
            int nth = thread_counts[tci];
            omp_set_num_threads(nth);
            double total_time = 0.0;

            for (int run=0; run<RUNS; ++run) {
                #pragma omp parallel for schedule(static)
                for (long long idx=0; idx<(long long)N*(long long)N; ++idx) C[idx] = 0.0;

                struct timeval t1,t2;
                gettimeofday(&t1, NULL);

                #pragma omp parallel for collapse(2) schedule(dynamic)
                for (int ii=0; ii<N; ii+=Bsize) {
                    for (int jj=0; jj<N; jj+=Bsize) {
                        for (int kk=0; kk<N; kk+=Bsize) {
                            int iimax = (ii + Bsize > N) ? N : ii + Bsize;
                            int jjmax = (jj + Bsize > N) ? N : jj + Bsize;
                            int kkmax = (kk + Bsize > N) ? N : kk + Bsize;
                            for (int i = ii; i < iimax; ++i) {
                                for (int k = kk; k < kkmax; ++k) {
                                    double a = A[(long long)i * N + k];
                                    long long baseC = (long long)i * N;
                                    long long baseB = (long long)k * N;
                                    for (int j = jj; j < jjmax; ++j) {
                                        C[baseC + j] += a * B[baseB + j];
                                    }
                                }
                            }
                        }
                    }
                }

                gettimeofday(&t2, NULL);
                double elapsed = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)*1e-6;
                total_time += elapsed;
                if (run == 0) {
                    printf("[threads=%2d] run %d time = %g s\n", nth, run+1, elapsed);
                }
            } 

            double avg_time = total_time / RUNS;
            if (nth == 1) baseline_time = avg_time;
            double speedup = (baseline_time > 0.0) ? baseline_time / avg_time : 1.0;
            printf("threads=%2d average_time=%g s   speedup=%.3g\n", nth, avg_time, speedup);

            fprintf(fout, "%d,%d,%g,%.3f\n", Bsize, nth, avg_time, speedup);
            fflush(fout);

        } 
    } 

    printf("\nCheck: C[0,0]=%g (expected ~N*average value)\n", C[0]);

    fclose(fout);
    free(A); free(B); free(C);
    return 0;
}
