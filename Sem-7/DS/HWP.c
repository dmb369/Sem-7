// Hello_World_Parallel.c
#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        printf("Dev %d\n", id);
        printf("Bandhiya %d\n", id);
    }
    return 0;
}
