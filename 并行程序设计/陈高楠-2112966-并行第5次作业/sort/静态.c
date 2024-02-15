#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define ARR_NUM 10000
#define ARR_LEN 10000
#define PROCESS_NUM 6
#define SEG (ARR_NUM / PROCESS_NUM)

int arr[ARR_NUM][ARR_LEN];

void init_2(void) {
    int i, j, ratio;
    srand((unsigned)time(NULL));
    for (i = 0; i < ARR_NUM; i++) {
        if (i < SEG) ratio = 0;
        else if (i < SEG * 2) ratio = 32;
        else ratio = 128;

        if ((rand() & 127) < ratio) {
            for (j = 0; j < ARR_LEN; j++) {
                arr[i][j] = ARR_LEN - j;
            }
        } else {
            for (j = 0; j < ARR_LEN; j++) {
                arr[i][j] = j;
            }
        }
    }
}

int compare_ints(const void* a, const void* b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

// Main function
int main() {
    int my_rank, comm_sz;
    double start, end;
    double time;
    
    // Initialize the array
    init_2();

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // Allocate tasks based on process number
    int startTask = my_rank * SEG;
    int endTask = startTask + SEG;

    // Sorting
    start = MPI_Wtime();
    for (int i = startTask; i < endTask && i < ARR_NUM; i++) {
        qsort(arr[i], ARR_LEN, sizeof(int), compare_ints);
    }
    end = MPI_Wtime();
    time = end - start;

    // Other processes send their runtime to process 0
    if (my_rank != 0) {
        MPI_Send(&time, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } else { // Process 0 prints the runtime of all processes
        printf("各进程运行时间如下:\n");
        printf("0号进程: %fs\n", time);
        for (int source = 1; source < comm_sz; source++) {
            MPI_Recv(&time, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%d号进程: %fs\n", source, time);
        }
    }

    MPI_Finalize();
    return 0;
}

