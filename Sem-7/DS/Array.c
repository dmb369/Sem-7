// Array.c
#include <stdio.h>
#include <omp.h>

int main() 
{
    int A[4] = {1,2,3,4};
    #pragma omp parallel num_threads(4)
    {
        int id = omp_get_thread_num();
        int B = A[id];
        printf("Value of B is %d\n",B);
    }
    return 0;
}
