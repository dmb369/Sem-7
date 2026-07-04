// File: convolution.c

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Function to perform serial convolution
void convolution_serial(float **input, float **kernel, float **output, int N, int M) 
{
    int kCenter = M / 2;
    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            float sum = 0.0f;
            for (int m = 0; m < M; m++) 
            {
                for (int n = 0; n < M; n++) 
                {
                    int x = i + (m - kCenter);
                    int y = j + (n - kCenter);
                    if (x >= 0 && x < N && y >= 0 && y < N) sum += input[x][y] * kernel[m][n];
                }
            }
            output[i][j] = sum;
        }
    }
}

// Function to perform parallel convolution
void convolution_parallel(float **input, float **kernel, float **output, int N, int M) 
{
    int kCenter = M / 2;
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) 
        {
            float sum = 0.0f;
            for (int m = 0; m < M; m++) 
            {
                for (int n = 0; n < M; n++) 
                {
                    int x = i + (m - kCenter);
                    int y = j + (n - kCenter);
                    if (x >= 0 && x < N && y >= 0 && y < N) sum += input[x][y] * kernel[m][n];
                }
            }
            output[i][j] = sum;
        }
    }
}

int main() 
{
    // Prompt user for image and kernel sizes
    int N, M;
    printf("Enter image dimensions (N*N) and kernel size (M*M): ");
    scanf("%d %d", &N, &M);

    // Allocate memory
    float **input = (float **)malloc(N * sizeof(float *));
    float **output_serial = (float **)malloc(N * sizeof(float *));
    float **output_parallel = (float **)malloc(N * sizeof(float *));

    for (int i = 0; i < N; i++) 
    {
        input[i] = (float *)malloc(N * sizeof(float));
        output_serial[i] = (float *)malloc(N * sizeof(float));
        output_parallel[i] = (float *)malloc(N * sizeof(float));
    }

    float **kernel = (float **)malloc(M * sizeof(float *));
    for (int i = 0; i < M; i++)
    {
        kernel[i] = (float *)malloc(M * sizeof(float));
    }

    // Initializing input and kernel with random values
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            input[i][j] = (float)(rand() % 10);
        }
    }

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < M; j++)
        {
            kernel[i][j] = (float)(rand() % 5);
        }
    }

    // Serial Execution
    double start_serial = omp_get_wtime();
    convolution_serial(input, kernel, output_serial, N, M);
    double end_serial = omp_get_wtime();
    double time_serial = end_serial - start_serial;
    printf("Serial Time: %.6f seconds\n", time_serial);

    // Parallel Execution 
    int thread_counts[] = {2, 4, 6, 8, 10, 12, 14, 16};
    int num_tests = sizeof(thread_counts) / sizeof(thread_counts[0]);

    printf("\nThreads\tParallel Time (s)\tSpeedup\n");

    for (int t = 0; t < num_tests; t++) 
    {
        int num_threads = thread_counts[t];
        omp_set_num_threads(num_threads);

        double start_parallel = omp_get_wtime();
        convolution_parallel(input, kernel, output_parallel, N, M);
        double end_parallel = omp_get_wtime();
        double time_parallel = end_parallel - start_parallel;

        double speedup = time_serial / time_parallel;
        printf("%d\t%.6f\t\t%.2f\n", num_threads, time_parallel, speedup);
    }


    // Free memory
    for (int i = 0; i < N; i++) 
    {
        free(input[i]);
        free(output_serial[i]);
        free(output_parallel[i]);
    }

    free(input);
    free(output_serial);
    free(output_parallel);
    for (int i = 0; i < M; i++) free(kernel[i]);
    free(kernel);

    return 0;
}
