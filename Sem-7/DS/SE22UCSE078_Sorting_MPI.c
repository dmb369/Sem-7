// File: SE22UCSE078_Sorting_MPI_CountingReduce.c
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int n_local = 1000000;
    const int K = 1000;
    int *local_data = malloc(n_local * sizeof(int));
    int local_count[K] = {0};
    int global_count[K] = {0};

    // Generate random integers
    srand(time(NULL) + rank * 1234);
    for (int i = 0; i < n_local; i++) 
    {
        int num = rand() % K;
        local_data[i] = num;
        local_count[num]++;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    
    // Combine counts from all processes
    MPI_Reduce(local_count, global_count, K, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) 
    {
        printf("Global counting sort complete.\n");

        // Reconstruct sorted array 
        long total = 0;
        for (int i = 0; i < K; i++) total += global_count[i];
        printf("Total elements = %ld\n", total);

        //To have the fully sorted list
        int *global_sorted = malloc(total * sizeof(int));
        long idx = 0;
        for (int i = 0; i < K; i++) 
        {
            for (int j = 0; j < global_count[i]; j++) 
            {
                global_sorted[idx++] = i;
            }
        }

        // Print first 100 sorted elements as a sample
        printf("First 100 sorted elements:\n");
        for (int i = 0; i < 100 && i < total; i++) 
        {
            printf("%d ", global_sorted[i]);
        }
      
        printf("\n"); 
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
